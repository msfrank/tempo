
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <tempo_config/config_result.h>
#include <tempo_config/config_utils.h>
#include <tempo_config/extend_map.h>
#include <tempo_config/internal/config_listener.h>
#include <tempo_config/internal/tracing_error_listener.h>
#include <tempo_utils/file_reader.h>
#include <tempo_utils/file_writer.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/log_stream.h>

#include <antlr4-runtime.h>

#include "ConfigLexer.h"
#include "ConfigParser.h"
#include "tempo_config/config_parser.h"

/**
 * Parse the given string as UTF-8 encoded JSON and deserialize it into a ConfigNode.
 *
 * @param bytes The source string containing UTF-8 encoded JSON.
 * @return A `tempo_config::Result` containing a ConfigNode if parsing was successful,
 *     otherwise a Result containing a Status describing the failure.
 */
tempo_utils::Result<tempo_config::ConfigNode>
tempo_config::read_config_string(std::string_view utf8, std::shared_ptr<ConfigSource> source)
{
    if (utf8.empty())
        return ConfigStatus::forCondition(ConfigCondition::kMissingValue, "empty config string");

    ConfigParserOptions options;
    ConfigParser parser(options);
    auto recorder = tempo_tracing::TraceRecorder::create();
    return parser.parseString(utf8, std::move(source), recorder);
}

/**
 * Read the config file at the specified path and parse its contents as UTF-8 encoded
 * JSON, then deserialize it into a ConfigNode.
 *
 * @param path The path to the config file.
 * @return A `tempo_config::Result` containing a ConfigNode if parsing was successful,
 *     otherwise a Result containing a Status describing the failure.
 */
tempo_utils::Result<tempo_config::ConfigNode>
tempo_config::read_config_file(const std::filesystem::path &path)
{
    ConfigParserOptions options;
    ConfigParser parser(options);
    auto recorder = tempo_tracing::TraceRecorder::create();
    return parser.parseFile(path, recorder);
}

/**
 * Read the config file at the specified path and parse its contents as a UTF-8 encoded
 * JSON object, then deserialize it into a ConfigMap.
 *
 * @param path The path to the config file.
 * @return A `tempo_config::Result` containing a ConfigMap if parsing was successful,
 *     otherwise a Result containing a Status describing the failure.
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::read_config_map_file(const std::filesystem::path &path)
{
    auto readConfigFileResult = read_config_file(path);
    if (readConfigFileResult.isStatus())
        return readConfigFileResult.getStatus();
    auto rootConfig = readConfigFileResult.getResult();

    if (rootConfig.getNodeType() != ConfigNodeType::kMap)
        return ConfigStatus::forCondition(
            ConfigCondition::kParseError,
            "invalid config in {}: root config value must be a map", path.string());
    return rootConfig.toMap();
}

/**
 * Read each config file in the specified directory and parse its contents as a UTF-8 encoded
 * JSON object, then merge all of the configs into a single ConfigMap. If extension is not empty,
 * then ignore files which do not match the specified extension. The function does not recursively
 * scan subdirectories for config files. If the same key is present in more than one JSON object
 * then this function fails indicating the conflict.
 *
 * @param directory The path to the config file.
 * @param extension The config file extension. If not empty then only files matching the specified
 *     extension will be parsed. <EM>The extension MUST start with a dot character <CODE>'.'</CODE></EM>.
 * @return A `tempo_config::Result` containing a ConfigMap if parsing was successful,
 *     otherwise a Result containing a Status describing the failure.
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::read_config_map_directory(const std::filesystem::path &directory, std::string_view extension)
{
    if (!std::filesystem::exists(directory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "config directory {} not found", directory.string());
    if (!std::filesystem::is_directory(directory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileExists,
            "{} is not a directory", directory.string());

    ConfigMap directoryConfig;

    for (auto const& entry : std::filesystem::directory_iterator{directory}) {
        if (!entry.is_regular_file())
            continue;
        auto path = entry.path();
        if (!extension.empty()) {
            if (!path.has_extension())
                continue;
            if (path.extension() != extension)
                continue;
        }

        auto readConfigMapResult = read_config_map_file(path);
        if (readConfigMapResult.isStatus())
            return readConfigMapResult.getStatus();
        auto currConfig = readConfigMapResult.getResult();

        auto extendMapResult = extend_map(directoryConfig, currConfig);
        if (extendMapResult.isStatus())
            return extendMapResult.getStatus();
        directoryConfig = extendMapResult.getResult();
    }

    return directoryConfig;
}

/**
 *
 * @param directory
 * @param extension
 * @return
 */
tempo_utils::Result<tempo_config::ConfigMap>
tempo_config::read_config_tree_directory(const std::filesystem::path &directory, std::string_view extension)
{
    if (!std::filesystem::exists(directory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "config directory {} not found", directory.string());
    if (!std::filesystem::is_directory(directory))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileExists,
            "{} is not a directory", directory.string());

    absl::flat_hash_map<std::string,ConfigNode> directoryEntries;

    for (auto const& entry : std::filesystem::directory_iterator{directory}) {
        auto stat = entry.status();

        if (stat.type() == std::filesystem::file_type::directory) {
            auto path = entry.path();

            // the config key is the full filename
            auto key = path.filename().stem().string();

            // parse the subdirectory and store the config at the specified config key
            auto readSubdirectoryResult = read_config_tree_directory(path, extension);
            if (readSubdirectoryResult.isStatus())
                return readSubdirectoryResult.getStatus();
            directoryEntries[key] = readSubdirectoryResult.getResult();

        } else if (stat.type() == std::filesystem::file_type::regular) {
            auto path = entry.path();

            // verify the file has an extension and it matches the specified extension, otherwise skip it
            if (!extension.empty()) {
                if (!path.has_extension())
                    continue;
                if (path.extension() != extension)
                    continue;
            }

            // the config key is the stem of the filename
            auto key = path.filename().stem().string();

            // parse the config file and store the config at the specified config key
            auto readConfigResult = read_config_file(path);
            if (readConfigResult.isStatus())
                return readConfigResult.getStatus();
            directoryEntries[key] = readConfigResult.getResult();

        } else {
            // FIXME: we could process symlinks but it would require tracking cycles
            TU_LOG_WARN << "ignoring config tree entry " << entry.path() << ": invalid file type";
        }

    }

    return ConfigMap(directoryEntries);
}

/*
 * construct a rapidjson value from a ConfigNode.
 */
static bool
convert_variant_to_rapidjson_value(
    const tempo_config::ConfigNode &node,
    rapidjson::Value &v,
    rapidjson::Document::AllocatorType &allocator)
{
    switch (node.getNodeType()) {
        case tempo_config::ConfigNodeType::kNil: {
            v.SetNull();
            break;
        }
        case tempo_config::ConfigNodeType::kValue: {
            auto s = node.toValue().getValue();
            v.SetString(s.data(), s.size(), allocator);
            break;
        }
        case tempo_config::ConfigNodeType::kSeq: {
            v.SetArray();
            auto seq = node.toSeq();
            for (auto iterator = seq.seqBegin(); iterator != seq.seqEnd(); iterator++) {
                rapidjson::Value member;
                if (!convert_variant_to_rapidjson_value(*iterator, member, allocator))
                    return false;
                v.PushBack(member, allocator);
            }
            break;
        }
        case tempo_config::ConfigNodeType::kMap: {
            v.SetObject();
            auto map = node.toMap();
            for (auto iterator = map.mapBegin(); iterator != map.mapEnd(); iterator++) {
                rapidjson::Value key;
                key.SetString(iterator->first.data(), iterator->first.size(), allocator);
                rapidjson::Value value;
                if (!convert_variant_to_rapidjson_value(iterator->second, value, allocator))
                    return false;
                v.AddMember(key, value, allocator);
            }
            break;
        }
        default:
            return false;
    }
    return true;
}

/**
 * Serialize the given ConfigNode as UTF-8 encoded JSON and store it in the specified bytes string.
 * If serialization fails then bytes is not modified.
 *
 * @param root The ConfigNode to write.
 * @param bytes The destination string used to store the serialized ConfigNode.
 * @return A `tempo_utils::Status` containing the status of the operation.
 */
tempo_utils::Status
tempo_config::write_config_string(const tempo_config::ConfigNode &root, std::string &bytes)
{
    rapidjson::Document doc;
    auto &allocator = doc.GetAllocator();
    convert_variant_to_rapidjson_value(root, doc, allocator);

    // serialize doc to buffer
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    // write the buffer to the supplied string
    bytes = std::string(buffer.GetString(), buffer.GetSize());
    return {};
}

/**
 * Serialize the given ConfigNode as UTF-8 encoded JSON and store it in the file at the specified
 * by path. If serialization fails then the file is not created or modified. Otherwise if serialization
 * is successful then the file will be overwritten if it exists, and created if it does not exist.
 *
 * @param root The ConfigNode to write.
 * @param path The destination file used to store the serialized ConfigNode.
 * @return A `tempo_utils::Status` containing the status of the operation.
 */
tempo_utils::Status
tempo_config::write_config_file(const tempo_config::ConfigNode &root, const std::filesystem::path &path)
{
    std::string bytes;
    auto status = write_config_string(root, bytes);
    if (status.notOk())
        return status;
    tempo_utils::FileWriter fileWriter(path, bytes, tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    if (!fileWriter.isValid())
        return fileWriter.getStatus();
    return {};
}
