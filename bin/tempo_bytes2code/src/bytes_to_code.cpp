
#include <absl/strings/str_cat.h>
#include <absl/time/time.h>
#include <absl/time/clock.h>

#include <tempo_command/command_help.h>
#include <tempo_command/command_parser.h>
#include <tempo_utils/file_writer.h>

#include "bytes_to_code_serializer.h"
#include "tempo_config/base_conversions.h"
#include "tempo_utils/file_reader.h"

inline const char *endline()
{
    return "\n";
}

/**
 * generate a C++ header file containing two variables, 'data' and 'size', and write the header file to
 * the specified path. The 'data' variable is declared as an uint8 array containing the specified bytes,
 * and the 'size' variable is declared as a uint32 containing the size of the data array.
 *
 * @param bytes
 * @param guard
 * @param ns
 * @param path
 * @return
 */
tempo_utils::Status
bytes_to_code(int argc, const char *argv[])
{
    tempo_config::PathParser inputDataFileParser;
    tempo_config::PathParser outputHeaderPathParser;
    tempo_config::StringParser includeGuardParser("");
    tempo_config::StringParser cppNamespaceParser("");
    tempo_config::IntegerParser verboseParser(0);
    tempo_config::IntegerParser quietParser(0);
    tempo_config::BooleanParser silentParser(false);

    std::vector<tempo_command::Default> cmdDefaults = {
        {"includeGuard", {}, "define a preprocessor variable to use as an include guard", "GUARD"},
        {"cppNamespace", {}, "declare the data and size variables with a namespace", "NAMESPACE"},
        {"verbose", verboseParser.getDefault(), "Display verbose output (specify twice for even more verbose output)"},
        {"quiet", quietParser.getDefault(), "Display warnings and errors only (specify twice for errors only)"},
        {"silent", silentParser.getDefault(), "Suppress all output"},
        {"inputDataFile", {}, "Path to the input data file", "FILE"},
        {"outputHeaderPath", {}, "Path to the output header file", "PATH"},
    };

    const std::vector<tempo_command::Grouping> cmdGroupings = {
        {"includeGuard", {"-g", "--include-guard"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"cppNamespace", {"-n", "--cpp-namespace"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"verbose", {"-v"}, tempo_command::GroupingType::NO_ARGUMENT},
        {"quiet", {"-q"}, tempo_command::GroupingType::NO_ARGUMENT},
        {"silent", {"-s", "--silent"}, tempo_command::GroupingType::NO_ARGUMENT},
        {"help", {"-h", "--help"}, tempo_command::GroupingType::HELP_FLAG},
        {"version", {"--version"}, tempo_command::GroupingType::VERSION_FLAG},
    };

    const std::vector<tempo_command::Mapping> optMappings = {
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "includeGuard"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "cppNamespace"},
        {tempo_command::MappingType::COUNT_INSTANCES, "verbose"},
        {tempo_command::MappingType::COUNT_INSTANCES, "quiet"},
        {tempo_command::MappingType::TRUE_IF_INSTANCE, "silent"},
    };

    std::vector<tempo_command::Mapping> argMappings = {
        {tempo_command::MappingType::ONE_INSTANCE, "inputDataFile"},
        {tempo_command::MappingType::ONE_INSTANCE, "outputHeaderPath"},
    };

    // parse argv array into a vector of tokens
    auto tokenizeResult = tempo_command::tokenize_argv(argc - 1, &argv[1]);
    if (tokenizeResult.isStatus())
        display_status_and_exit(tokenizeResult.getStatus());
    auto tokens = tokenizeResult.getResult();

    std::string subcommand;
    tempo_command::OptionsHash cmdOptions;
    tempo_command::ArgumentVector cmdArguments;

    // parse global options and get the subcommand
    auto status = tempo_command::parse_completely(tokens, cmdGroupings, cmdOptions, cmdArguments);
    if (status.notOk()) {
        tempo_command::CommandStatus commandStatus;
        if (!status.convertTo(commandStatus))
            return status;
        switch (commandStatus.getCondition()) {
            case tempo_command::CommandCondition::kHelpRequested:
                tempo_command::display_help_and_exit({"tempo-bytes2code"},
                    "Generate a C/C++ header file containing the specified input as a byte array",
                    {}, cmdGroupings, optMappings, argMappings, cmdDefaults);
            default:
                return status;
        }
    }

    // initialize the global config from defaults
    auto cmdConfig = command_config_from_defaults(cmdDefaults);

    // convert options to config
    TU_RETURN_IF_NOT_OK (convert_options(cmdOptions, optMappings, cmdConfig));

    // convert arguments to config
    TU_RETURN_IF_NOT_OK (convert_arguments(cmdArguments, argMappings, cmdConfig));

    TU_LOG_INFO << "cmd config:\n" << tempo_command::command_config_to_string(cmdConfig);

    // configure logging
    tempo_utils::LoggingConfiguration logging = {
        tempo_utils::SeverityFilter::kDefault,
        true,
        false,
        false,
    };

    bool silent;
    TU_RETURN_IF_NOT_OK(tempo_command::parse_command_config(silent, silentParser,
        cmdConfig, "silent"));
    if (silent) {
        logging.severityFilter = tempo_utils::SeverityFilter::kSilent;
    } else {
        int verbose, quiet;
        TU_RETURN_IF_NOT_OK(tempo_command::parse_command_config(verbose, verboseParser,
            cmdConfig, "verbose"));
        TU_RETURN_IF_NOT_OK(tempo_command::parse_command_config(quiet, quietParser,
            cmdConfig, "quiet"));
        if (verbose && quiet)
            return tempo_command::CommandStatus::forCondition(
                tempo_command::CommandCondition::kCommandError, "cannot specify both -v and -q");
        if (verbose == 1) {
            logging.severityFilter = tempo_utils::SeverityFilter::kVerbose;
        } else if (verbose > 1) {
            logging.severityFilter = tempo_utils::SeverityFilter::kVeryVerbose;
        }
        if (quiet == 1) {
            logging.severityFilter = tempo_utils::SeverityFilter::kWarningsAndErrors;
        } else if (quiet > 1) {
            logging.severityFilter = tempo_utils::SeverityFilter::kErrorsOnly;
        }
    }

    // initialize logging
    tempo_utils::init_logging(logging);

    // determine the input data file
    std::filesystem::path inputDataFile;
    TU_RETURN_IF_NOT_OK(tempo_command::parse_command_config(inputDataFile, inputDataFileParser,
        cmdConfig, "inputDataFile"));

    // determine the output header path
    std::filesystem::path outputHeaderPath;
    TU_RETURN_IF_NOT_OK(tempo_command::parse_command_config(outputHeaderPath, outputHeaderPathParser,
        cmdConfig, "outputHeaderPath"));

    // determine the include guard
    std::string includeGuard;
    TU_RETURN_IF_NOT_OK(tempo_command::parse_command_config(includeGuard, includeGuardParser,
        cmdConfig, "includeGuard"));

    // determine the output header path
    std::string cppNamespace;
    TU_RETURN_IF_NOT_OK(tempo_command::parse_command_config(cppNamespace, cppNamespaceParser,
        cmdConfig, "cppNamespace"));

    // serialize the input
    BytesToCodeSerializer serializer(inputDataFile, includeGuard, cppNamespace);
    TU_RETURN_IF_NOT_OK (serializer.getStatus());

    // write the code to the path, overwriting if a file already exists at the specified path
    tempo_utils::FileWriter writer(outputHeaderPath, serializer.getCode(), tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    TU_RETURN_IF_NOT_OK (writer.getStatus());

    return {};
}

int
main(int argc, const char *argv[]) {
    if (argc == 0 || argv == nullptr)
        return -1;

    auto status = bytes_to_code(argc, argv);
    if (!status.isOk())
        tempo_command::display_status_and_exit(status);
    return 0;
}
