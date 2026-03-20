
#include <tempo_command/command_conversions.h>
#include <tempo_command/command_result.h>
#include <tempo_command/logging_environment.h>
#include <tempo_config/base_conversions.h>
#include <tempo_config/config_types.h>
#include <tempo_config/config_utils.h>
#include <tempo_config/merge_map.h>
#include <tempo_config/parse_config.h>
#include <tempo_utils/logging.h>
#include <tempo_utils/log_sink.h>
#include <tempo_utils/result.h>

static tempo_utils::Result<tempo_config::ConfigMap>
load_logging_override_config()
{
    const auto *value = std::getenv(tempo_command::kEnvOverrideLoggingConfigName);
    if (value == nullptr)
        return tempo_config::ConfigMap{};
    tempo_config::ConfigNode overrideNode;
    TU_ASSIGN_OR_RETURN (overrideNode, tempo_config::read_config_string(value));

    if (overrideNode.getNodeType() != tempo_config::ConfigNodeType::kMap)
        return tempo_command::CommandStatus::forCondition(tempo_command::CommandCondition::kInvalidConfiguration,
            "invalid type for environment variable {}; expected a map",
            tempo_command::kEnvOverrideLoggingConfigName);

    return overrideNode.toMap();
}

static tempo_utils::Result<tempo_config::ConfigMap>
load_logging_config_file(const std::filesystem::path &loggingConfigFile)
{
    std::filesystem::path path;

    if (loggingConfigFile.empty()) {
        const auto *value = std::getenv(tempo_command::kEnvLoggingConfigFileName);
        if (value == nullptr)
            return tempo_config::ConfigMap{};
        path = std::filesystem::path{value};
    } else {
        path = loggingConfigFile;
    }

    tempo_config::ConfigNode loggingNode;
    TU_ASSIGN_OR_RETURN (loggingNode, tempo_config::read_config_file(path));

    if (loggingNode.getNodeType() != tempo_config::ConfigNodeType::kMap)
        return tempo_command::CommandStatus::forCondition(
            tempo_command::CommandCondition::kInvalidConfiguration,
            "invalid logging configuration in {}; expected a map",
            loggingConfigFile.string());

    return loggingNode.toMap();
}

static tempo_utils::Status
parse_default_log_sink(
    const tempo_config::ConfigMap &loggingMap,
    std::unique_ptr<tempo_utils::AbstractLogSink> &logSink)
{
    tempo_config::BooleanParser displayShortFormParser(false);
    bool displayShortForm;
    TU_RETURN_IF_NOT_OK (tempo_config::parse_config(displayShortForm, displayShortFormParser,
        loggingMap, "displayShortForm"));

    tempo_config::BooleanParser logToStdoutParser(false);
    bool logToStdout;
    TU_RETURN_IF_NOT_OK (tempo_config::parse_config(logToStdout, logToStdoutParser,
        loggingMap, "logToStdout"));

    logSink = std::make_unique<tempo_utils::DefaultLogSink>(displayShortForm, logToStdout);
    return {};
}

static tempo_utils::Status
parse_log_file_sink(
    const tempo_config::ConfigMap &loggingMap,
    std::unique_ptr<tempo_utils::AbstractLogSink> &logSink)
{
    tempo_config::BooleanParser displayShortFormParser(false);
    bool displayShortForm;
    TU_RETURN_IF_NOT_OK (tempo_config::parse_config(displayShortForm, displayShortFormParser,
        loggingMap, "displayShortForm"));

    tempo_config::PathParser logFilePathParser;
    std::filesystem::path logFilePath;
    TU_RETURN_IF_NOT_OK (tempo_config::parse_config(logFilePath, logFilePathParser,
        loggingMap, "logFilePath"));

    logSink = std::make_unique<tempo_utils::LogFileSink>(logFilePath, displayShortForm);
    return {};
}

static tempo_utils::Status
parse_custom_log_sink(
    const tempo_config::ConfigMap &loggingMap,
    std::unique_ptr<tempo_utils::AbstractLogSink> &logSink)
{
    return tempo_command::CommandStatus::forCondition(
        tempo_command::CommandCondition::kInvalidConfiguration,
        "custom log sink is not supported");
}

static tempo_utils::Status
parse_logging_config(
    const tempo_config::ConfigMap &loggingMap,
    bool displayShortForm,
    bool logToStdout,
    tempo_utils::LoggingConfiguration &loggingConfig,
    std::unique_ptr<tempo_utils::AbstractLogSink> &logSink)
{
    if (loggingMap.mapEmpty()) {
        loggingConfig.severityFilter = tempo_utils::SeverityFilter::kDefault;
        loggingConfig.flushEveryMessage = false;
        logSink = std::make_unique<tempo_utils::DefaultLogSink>(displayShortForm, logToStdout);
        return {};
    }

    tempo_config::BooleanParser flushEveryMessageParser(false);
    TU_RETURN_IF_NOT_OK (tempo_config::parse_config(loggingConfig.flushEveryMessage, flushEveryMessageParser,
        loggingMap, "flushEveryMessage"));

    tempo_command::SeverityFilterParser severityFilterParser(tempo_utils::SeverityFilter::kDefault);
    TU_RETURN_IF_NOT_OK (tempo_config::parse_config(loggingConfig.severityFilter, severityFilterParser,
        loggingMap, "severityFilter"));

    auto logSinkNode = loggingMap.mapAt("logSink");
    auto logSinkNodeType = logSinkNode.getNodeType();
    switch (logSinkNodeType) {
        case tempo_config::ConfigNodeType::kValue: {
            auto logSinkType = logSinkNode.toValue().getValue();
            if (logSinkType == "Default")
                return parse_default_log_sink(loggingMap, logSink);
            if (logSinkType == "LogFile")
                return parse_log_file_sink(loggingMap, logSink);
            return tempo_command::CommandStatus::forCondition(
                tempo_command::CommandCondition::kInvalidConfiguration,
                "invalid logging configuration for 'logSink'; '{}' is not a valid log sink",
                logSinkNode.toString());
        }
        case tempo_config::ConfigNodeType::kMap:
            return parse_custom_log_sink(loggingMap, logSink);
        default:
            return tempo_command::CommandStatus::forCondition(
                tempo_command::CommandCondition::kInvalidConfiguration,
                "invalid logging configuration 'logSink'; value must be a string or map",
                logSinkNode.toString());
    }
}

tempo_utils::Status
tempo_command::init_logging_from_environment(
    const std::filesystem::path &loggingConfigFile,
    const tempo_config::ConfigMap &loggingOverrides,
    bool displayShortForm,
    bool logToStdout)
{
    tempo_config::ConfigMap configMap;
    TU_ASSIGN_OR_RETURN (configMap, load_logging_config_file(loggingConfigFile));

    tempo_config::ConfigMap envMap;
    TU_ASSIGN_OR_RETURN (envMap, load_logging_override_config());

    auto loggingMap = tempo_config::merge_map(
        tempo_config::merge_map(configMap, envMap), loggingOverrides);

    tempo_utils::LoggingConfiguration loggingConfig;
    std::unique_ptr<tempo_utils::AbstractLogSink> logSink;

    TU_RETURN_IF_NOT_OK (parse_logging_config(
        loggingMap, displayShortForm,logToStdout, loggingConfig, logSink));

    return tempo_utils::init_logging(loggingConfig, std::move(logSink));
}
