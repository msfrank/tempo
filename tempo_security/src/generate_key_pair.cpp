
#include <project_config.h>

#include <tempo_command/command_help.h>
#include <tempo_command/command_parser.h>
#include <tempo_security/certificate_key_pair.h>
#include <tempo_config/base_conversions.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/url.h>

tempo_utils::Status
run(int argc, char *argv[])
{
    tempo_config::PathParser installDirectoryParser(std::filesystem::current_path());
    tempo_config::PathParser signingCertificateParser(std::filesystem::path{});
    tempo_config::PathParser signingPrivateKeyParser(std::filesystem::path{});
    tempo_config::StringParser fileNameParser(std::string{});
    tempo_config::StringParser organizationParser(std::string{});
    tempo_config::StringParser organizationalUnitParser(std::string{});
    tempo_config::StringParser commonNameParser;
    tempo_config::IntegerParser validitySecondsParser(60*60*24*365);
    tempo_config::IntegerParser serialNumberParser;
    tempo_config::IntegerParser pathlenParser(-1);
    tempo_config::BooleanParser isCAParser(false);
    tempo_config::BooleanParser isSelfSignedParser(false);

    std::vector<tempo_command::Default> defaults = {
        {"installDirectory", {}, "install directory", "DIR"},
        {"signingCertificate", {}, "signing certificate file", "FILE"},
        {"signingPrivateKey", {}, "signing private key file", "FILE"},
        {"fileName", {}, "the keypair file name prefix", "NAME"},
        {"organization", {}, "the subject organization name", "NAME"},
        {"organizationalUnit", {}, "the subject organizational unit name", "NAME"},
        {"commonName", {}, "the subject common name", "NAME"},
        {"validitySeconds", {}, "the duration in which the certificate is valid", "SECONDS"},
        {"serialNumber", {}, "the certificate serial number", "SERIAL"},
        {"pathlen", {}, "the certificate serial number", "NUM"},
        {"isCA", {}, "the certificate should be a CA", {}},
        {"isSelfSigned", {}, "the certificate should be self-signed", {}},
    };

    std::vector<tempo_command::Grouping> groupings = {
        {"installDirectory", {"--install-directory"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"signingCertificate", {"--signing-certificate"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"signingPrivateKey", {"--signing-private-key"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"fileName", {"--file-name"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"organization", {"--organization"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"organizationalUnit", {"--organizational-unit"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"commonName", {"--common-name"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"validitySeconds", {"--validity"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"serialNumber", {"--serial"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"pathlen", {"--pathlen"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"isCA", {"--ca"}, tempo_command::GroupingType::NO_ARGUMENT},
        {"isSelfSigned", {"--self-signed"}, tempo_command::GroupingType::NO_ARGUMENT},
        {"help", {"-h", "--help"}, tempo_command::GroupingType::HELP_FLAG},
        {"version", {"--version"}, tempo_command::GroupingType::VERSION_FLAG},
    };

    std::vector<tempo_command::Mapping> optMappings = {
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "installDirectory"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "signingCertificate"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "signingPrivateKey"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "fileName"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "organization"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "organizationalUnit"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "commonName"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "validitySeconds"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "serialNumber"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "pathlen"},
        {tempo_command::MappingType::TRUE_IF_INSTANCE, "isCA"},
        {tempo_command::MappingType::TRUE_IF_INSTANCE, "isSelfSigned"},
    };

    std::vector<tempo_command::Mapping> argMappings;

    tempo_command::OptionsHash options;
    tempo_command::ArgumentVector arguments;

    tempo_command::CommandConfig config = command_config_from_defaults(defaults);

    // parse argv array into a vector of tokens
    auto tokenizeResult = tempo_command::tokenize_argv(argc - 1, &argv[1]);
    if (tokenizeResult.isStatus())
        display_status_and_exit(tokenizeResult.getStatus());
    auto tokens = tokenizeResult.getResult();

    // parse remaining options and arguments
    auto status = tempo_command::parse_completely(tokens, groupings, options, arguments);
    if (status.notOk()) {
        tempo_command::CommandStatus commandStatus;
        if (!status.convertTo(commandStatus))
            return status;
        switch (commandStatus.getCondition()) {
            case tempo_command::CommandCondition::kHelpRequested:
                tempo_command::display_help_and_exit({"generate-key-pair"},
                    "generate a key-pair",
                    {}, groupings, optMappings, argMappings, defaults);
            default:
                return status;
        }
    }

    // convert options to config
    status = convert_options(options, optMappings, config);
    if (!status.isOk())
        return status;

    // convert arguments to config
    status = convert_arguments(arguments, argMappings, config);
    if (!status.isOk())
        return status;

    TU_LOG_INFO << "config:\n" << tempo_command::command_config_to_string(config);

    // determine the install directory
    std::filesystem::path installDirectory;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(installDirectory, installDirectoryParser,
        config, "installDirectory"));

    // determine the signing certificate path
    std::filesystem::path signingCertificateFile;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(signingCertificateFile, signingCertificateParser,
        config, "signingCertificate"));

    // determine the signing key path
    std::filesystem::path signingPrivateKeyFile;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(signingPrivateKeyFile, signingPrivateKeyParser,
        config, "signingPrivateKey"));

    // determine the file name stem
    std::string fileName;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(fileName, fileNameParser,
        config, "fileName"));

    // determine the organization
    std::string organization;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(organization, organizationParser,
        config, "organization"));

    // determine the organizational unit
    std::string organizationalUnit;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(organizationalUnit, organizationalUnitParser,
        config, "organizationalUnit"));

    // determine the common name
    std::string commonName;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(commonName, commonNameParser,
        config, "commonName"));

    // determine the validity
    int validitySeconds;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(validitySeconds, validitySecondsParser,
        config, "validitySeconds"));

    // determine the serial number
    int serialNumber;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(serialNumber, serialNumberParser,
        config, "serialNumber"));

    // determine the serial number
    int pathlen;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(pathlen, pathlenParser, config, "pathlen"));

    // determine if CA
    bool isCA;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(isCA, isCAParser, config, "isCA"));

    // determine if self signed
    bool isSelfSigned;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(isSelfSigned, isSelfSignedParser,
        config, "isSelfSigned"));

    tempo_security::CertificateKeyPair keyPair;

    //
    if (isCA) {
        if (isSelfSigned) {
            keyPair = tempo_security::generate_self_signed_rsa_ca_key_pair(
                organization,
                organizationalUnit,
                commonName,
                serialNumber,
                std::chrono::seconds(validitySeconds),
                pathlen,
                installDirectory,
                fileName);
        } else {
            tempo_security::CertificateKeyPair caKeyPair(signingPrivateKeyFile, signingCertificateFile);
            keyPair = tempo_security::generate_rsa_ca_key_pair(
                caKeyPair,
                organization,
                organizationalUnit,
                commonName,
                serialNumber,
                std::chrono::seconds(validitySeconds),
                pathlen,
                installDirectory,
                fileName);
        }
    }
    else {
        if (isSelfSigned) {
            keyPair = tempo_security::generate_self_signed_rsa_key_pair(
                organization,
                organizationalUnit,
                commonName,
                serialNumber,
                std::chrono::seconds(validitySeconds),
                installDirectory,
                fileName);
        } else {
            tempo_security::CertificateKeyPair caKeyPair(signingPrivateKeyFile, signingCertificateFile);
            keyPair = tempo_security::generate_rsa_key_pair(
                caKeyPair,
                organization,
                organizationalUnit,
                commonName,
                serialNumber,
                std::chrono::seconds(validitySeconds),
                installDirectory,
                fileName);
        }
    }

    if (!keyPair.isValid())
        return tempo_command::CommandStatus::commandFailure("failed to generate key-pair");
    TU_CONSOLE_OUT << "generated key-pair " << fileName << " in " << installDirectory;

    return tempo_command::CommandStatus::ok();
}

int
main(int argc, char *argv[], char *envp[])
{
    if (argc == 0 || argv == nullptr)
        return -1;

    auto status = run(argc, argv);
    if (!status.isOk())
        tempo_command::display_status_and_exit(status, 1);
    return 0;
}
