
#include <tempo_command/command_help.h>
#include <tempo_command/command_parser.h>
#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/ecdsa_private_key_generator.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_config/base_conversions.h>
#include <tempo_config/enum_conversions.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/url.h>

enum class KeyType { ECC, RSA, };

static std::unique_ptr<tempo_security::AbstractPrivateKeyGenerator>
create_key_generator(KeyType keyType)
{
    switch (keyType) {
        case KeyType::ECC: {
            tempo_security::CurveId curveId = tempo_security::CurveId::Prime256v1;
            return std::make_unique<tempo_security::EcdsaPrivateKeyGenerator>(curveId);
        }
        case KeyType::RSA: {
            int keyBits = tempo_security::kRSAKeyBits;
            int publicExponent = tempo_security::kRSAPublicExponent;
            return std::make_unique<tempo_security::RSAPrivateKeyGenerator>(keyBits, publicExponent);
        }
        default:
            return {};
    }
}

tempo_utils::Status
run(int argc, const char *argv[])
{
    tempo_config::PathParser outputDirectoryParser(std::filesystem::current_path());
    tempo_config::PathParser signingCertificateParser(std::filesystem::path{});
    tempo_config::PathParser signingPrivateKeyParser(std::filesystem::path{});
    tempo_config::StringParser fileNameParser(std::string{});
    tempo_config::StringParser organizationParser;
    tempo_config::StringParser organizationalUnitParser;
    tempo_config::StringParser commonNameParser;
    tempo_config::IntegerParser validitySecondsParser(60*60*24*365);
    tempo_config::IntegerParser serialNumberParser;
    tempo_config::IntegerParser pathlenParser(-1);
    tempo_config::BooleanParser isCAParser(false);
    tempo_config::BooleanParser isSelfSignedParser(false);

    tempo_config::EnumTParser<KeyType> keyTypeParser({
        {"ECC", KeyType::ECC},
        {"RSA", KeyType::RSA},
    });

    tempo_config::EnumTParser<tempo_security::DigestId> digestIdParser({
        {"None", tempo_security::DigestId::None},
        {"SHA256", tempo_security::DigestId::SHA256},
        {"SHA384", tempo_security::DigestId::SHA384},
        {"SHA512", tempo_security::DigestId::SHA512},
        {"SHA3_256", tempo_security::DigestId::SHA3_256},
        {"SHA3_384", tempo_security::DigestId::SHA3_384},
        {"SHA3_512", tempo_security::DigestId::SHA3_512},
    }, tempo_security::DigestId::None);

    std::vector<tempo_command::Default> defaults = {
        {"keyType", "private key type", "TYPE"},
        {"digestId", "message digest id", "ID"},
        {"outputDirectory", "the output directory", "DIR"},
        {"signingCertificate", "signing certificate file", "FILE"},
        {"signingPrivateKey", "signing private key file", "FILE"},
        {"fileName", "the keypair file name prefix", "NAME"},
        {"organization", "the subject organization name", "NAME"},
        {"organizationalUnit", "the subject organizational unit name", "NAME"},
        {"commonName", "the subject common name", "NAME"},
        {"validitySeconds", "the duration in which the certificate is valid", "SECONDS"},
        {"serialNumber", "the certificate serial number", "SERIAL"},
        {"pathlen", "the path length constraint on the CA certificate", "LENGTH"},
        {"isCA", "the certificate should be a CA"},
        {"isSelfSigned", "the certificate should be self-signed"},
    };

    std::vector<tempo_command::Grouping> groupings = {
        {"keyType", {"-t", "--key-type"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"digestId", {"--digest-id"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"outputDirectory", {"-o", "--output-directory"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"signingCertificate", {"--signing-certificate"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"signingPrivateKey", {"--signing-private-key"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"fileName", {"--file-name"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"organization", {"--organization"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"organizationalUnit", {"--organizational-unit"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"commonName", {"--common-name"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"validitySeconds", {"--validity"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"serialNumber", {"--serial"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"pathlen", {"--path-length"}, tempo_command::GroupingType::SINGLE_ARGUMENT},
        {"isCA", {"--ca"}, tempo_command::GroupingType::NO_ARGUMENT},
        {"isSelfSigned", {"--self-signed"}, tempo_command::GroupingType::NO_ARGUMENT},
        {"help", {"-h", "--help"}, tempo_command::GroupingType::HELP_FLAG},
        {"version", {"--version"}, tempo_command::GroupingType::VERSION_FLAG},
    };

    std::vector<tempo_command::Mapping> optMappings = {
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "keyType"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "digestId"},
        {tempo_command::MappingType::ZERO_OR_ONE_INSTANCE, "outputDirectory"},
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
    tempo_command::CommandConfig config;

    // parse argv array into a vector of tokens
    auto tokenizeResult = tempo_command::tokenize_argv(argc - 1, &argv[1]);
    if (tokenizeResult.isStatus())
        tempo_command::display_status_and_exit(tokenizeResult.getStatus());
    auto tokens = tokenizeResult.getResult();

    // parse remaining options and arguments
    auto status = tempo_command::parse_completely(tokens, groupings, options, arguments);
    if (status.notOk()) {
        tempo_command::CommandStatus commandStatus;
        if (!status.convertTo(commandStatus))
            return status;
        switch (commandStatus.getCondition()) {
            case tempo_command::CommandCondition::kHelpRequested:
                tempo_command::display_help_and_exit({"tempo-generate-keypair"},
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

    // determine the key type
    KeyType keyType;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(keyType, keyTypeParser,
        config, "keyType"));

    // determine the digest id
    tempo_security::DigestId digestId;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(digestId, digestIdParser,
        config, "digestId"));

    // determine the output directory
    std::filesystem::path outputDirectory;
    TU_RETURN_IF_NOT_OK (tempo_command::parse_command_config(outputDirectory, outputDirectoryParser,
        config, "outputDirectory"));

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

    // construct the key generator based on the key type
    auto keygen = create_key_generator(keyType);

    tempo_security::CertificateKeyPair keyPair;

    //
    if (isCA) {
        if (isSelfSigned) {
            TU_ASSIGN_OR_RETURN (keyPair, tempo_security::GenerateUtils::generate_self_signed_ca_key_pair(
                *keygen,
                digestId,
                organization,
                organizationalUnit,
                commonName,
                serialNumber,
                std::chrono::seconds(validitySeconds),
                pathlen,
                outputDirectory,
                fileName));
        } else {
            tempo_security::CertificateKeyPair caKeyPair;
            TU_ASSIGN_OR_RETURN (caKeyPair, tempo_security::CertificateKeyPair::load(
                signingPrivateKeyFile, signingCertificateFile));
            TU_ASSIGN_OR_RETURN (keyPair, tempo_security::GenerateUtils::generate_ca_key_pair(
                caKeyPair,
                *keygen,
                digestId,
                organization,
                organizationalUnit,
                commonName,
                serialNumber,
                std::chrono::seconds(validitySeconds),
                pathlen,
                outputDirectory,
                fileName));
        }
    }
    else {
        if (isSelfSigned) {
            TU_ASSIGN_OR_RETURN (keyPair, tempo_security::GenerateUtils::generate_self_signed_key_pair(
                *keygen,
                digestId,
                organization,
                organizationalUnit,
                commonName,
                serialNumber,
                std::chrono::seconds(validitySeconds),
                outputDirectory,
                fileName));
        } else {
            tempo_security::CertificateKeyPair caKeyPair;
            TU_ASSIGN_OR_RETURN (caKeyPair, tempo_security::CertificateKeyPair::load(
                signingPrivateKeyFile, signingCertificateFile));
            TU_ASSIGN_OR_RETURN (keyPair, tempo_security::GenerateUtils::generate_key_pair(
                caKeyPair,
                *keygen,
                digestId,
                organization,
                organizationalUnit,
                commonName,
                serialNumber,
                std::chrono::seconds(validitySeconds),
                outputDirectory,
                fileName));
        }
    }

    if (!keyPair.isValid())
        return tempo_command::CommandStatus::forCondition(
            tempo_command::CommandCondition::kCommandError, "failed to generate key-pair");

    TU_CONSOLE_OUT << "generated key-pair " << fileName << " in " << outputDirectory;

    return {};
}

int
main(int argc, const char *argv[], char *envp[])
{
    if (argc == 0 || argv == nullptr)
        return -1;

    auto status = run(argc, argv);
    if (!status.isOk())
        tempo_command::display_status_and_exit(status, 1);
    return 0;
}
