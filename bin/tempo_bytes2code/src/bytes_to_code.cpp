
#include <absl/strings/str_cat.h>
#include <absl/time/time.h>
#include <absl/time/clock.h>

#include <tempo_command/command_help.h>
#include <tempo_command/command_parser.h>
#include <tempo_utils/file_writer.h>

#include "bytes_to_code_serializer.h"
#include "tempo_command/command.h"
#include "tempo_config/base_conversions.h"
#include "tempo_utils/file_reader.h"
#include "tempo_utils/log_sink.h"

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

    tempo_command::Command command("tempo-bytes2code");

    command.addOption("includeGuard", {"-g", "--include-guard"},
        tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "Define a preprocessor variable to use as an include guard", "GUARD");
    command.addOption("cppNamespace", {"-n", "--cpp-namespace"},
        tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "Declare the data an size variables with a namespace", "NAMESPACE");
    command.addFlag("verbose", {"-v"}, tempo_command::MappingType::COUNT_INSTANCES,
        "Display verbose output (specify twice for even more verbose output)");
    command.addFlag("quiet", {"-q"}, tempo_command::MappingType::COUNT_INSTANCES,
        "Display warnings and errors only (specify twice for errors only)");
    command.addFlag("silent", {"-s"}, tempo_command::MappingType::TRUE_IF_INSTANCE,
        "Suppress all output");
    command.addArgument("inputDataFile", "FILE", tempo_command::MappingType::ONE_INSTANCE,
        "Path to the input data file");
    command.addArgument("outputHeaderPath", "PATH", tempo_command::MappingType::ONE_INSTANCE,
        "Path to the output header file");
    command.addHelpOption("help", {"-h", "--help"},
        "Generate a C/C++ header file containing the specified input as a byte array");

    TU_RETURN_IF_NOT_OK (command.parse(argc - 1, &argv[1]));

    // configure logging
    tempo_utils::LoggingConfiguration logging = {
        tempo_utils::SeverityFilter::kDefault,
        true,
    };

    bool silent;
    TU_RETURN_IF_NOT_OK(command.convert(silent, silentParser, "silent"));
    if (silent) {
        logging.severityFilter = tempo_utils::SeverityFilter::kSilent;
    } else {
        int verbose, quiet;
        TU_RETURN_IF_NOT_OK(command.convert(verbose, verboseParser, "verbose"));
        TU_RETURN_IF_NOT_OK(command.convert(quiet, quietParser, "quiet"));
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
    TU_RETURN_IF_NOT_OK(command.convert(inputDataFile, inputDataFileParser, "inputDataFile"));

    // determine the output header path
    std::filesystem::path outputHeaderPath;
    TU_RETURN_IF_NOT_OK(command.convert(outputHeaderPath, outputHeaderPathParser, "outputHeaderPath"));

    // determine the include guard
    std::string includeGuard;
    TU_RETURN_IF_NOT_OK(command.convert(includeGuard, includeGuardParser, "includeGuard"));

    // determine the output header path
    std::string cppNamespace;
    TU_RETURN_IF_NOT_OK(command.convert(cppNamespace, cppNamespaceParser, "cppNamespace"));

    // serialize the input
    BytesToCodeSerializer serializer(inputDataFile, includeGuard, cppNamespace);
    TU_RETURN_IF_NOT_OK (serializer.getStatus());

    // write the code to the path, overwriting if a file already exists at the specified path
    tempo_utils::FileWriter writer(outputHeaderPath,
        serializer.getCode(), tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    TU_RETURN_IF_NOT_OK (writer.getStatus());

    return {};
}

int
main(int argc, const char *argv[])
{
    if (argc == 0 || argv == nullptr)
        return -1;

    auto status = bytes_to_code(argc, argv);
    if (!status.isOk())
        tempo_command::display_status_and_exit(status);
    return 0;
}
