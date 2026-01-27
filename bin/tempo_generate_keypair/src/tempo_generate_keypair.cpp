
#include <tempo_command/command.h>
#include <tempo_config/base_conversions.h>
#include <tempo_config/enum_conversions.h>
#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/ecdsa_private_key_generator.h>
#include <tempo_security/ed25519_private_key_generator.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/url.h>


enum class KeyType { Ecdsa, Ed25519, Rsa, };

static std::unique_ptr<tempo_security::AbstractPrivateKeyGenerator>
create_key_generator(KeyType keyType)
{
    switch (keyType) {
        case KeyType::Ecdsa: {
            tempo_security::CurveId curveId = tempo_security::CurveId::Prime256v1;
            return std::make_unique<tempo_security::EcdsaPrivateKeyGenerator>(curveId);
        }
        case KeyType::Ed25519: {
            return std::make_unique<tempo_security::Ed25519PrivateKeyGenerator>();
        }
        case KeyType::Rsa: {
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
        {"ecdsa", KeyType::Ecdsa},
        {"ed25519", KeyType::Ed25519},
        {"rsa", KeyType::Rsa},
    });

    tempo_config::EnumTParser<tempo_security::DigestId> digestIdParser({
        {"none", tempo_security::DigestId::None},
        {"sha256", tempo_security::DigestId::SHA256},
        {"sha384", tempo_security::DigestId::SHA384},
        {"sha512", tempo_security::DigestId::SHA512},
        {"sha3_256", tempo_security::DigestId::SHA3_256},
        {"sha3_384", tempo_security::DigestId::SHA3_384},
        {"sha3_512", tempo_security::DigestId::SHA3_512},
    }, tempo_security::DigestId::None);

    tempo_command::Command command("tempo-generate-keypair");

    command.addOption("keyType", {"-t", "--key-type"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "Private key type", "TYPE");
    command.addOption("digestId", {"--digest-id"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "Message digest ID", "ID");
    command.addOption("outputDirectory", {"-o", "--output-directory"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "The output directory", "DIR");
    command.addOption("signingCertificate", {"--signing-certificate"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "Signing certificate file", "FILE");
    command.addOption("signingPrivateKey", {"--signing-private-key"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "Signing private key file", "FILE");
    command.addOption("fileName", {"--file-name"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "The keypair filename prefix", "NAME");
    command.addOption("organization", {"--organization"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "The subject organization name", "NAME");
    command.addOption("organizationalUnit", {"--organizational-unit"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "The subject organizational unit name", "NAME");
    command.addOption("commonName", {"--common-name"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "The subject common name", "NAME");
    command.addOption("validitySeconds", {"--validity"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "The duration in which the certificate is valid", "SECONDS");
    command.addOption("serialNumber", {"--serial"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "The certificate serial number", "SERIAL");
    command.addOption("pathlen", {"--path-length"}, tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "The path length constraint on the CA certificate", "LENGTH");
    command.addFlag("isCA", {"--ca"}, tempo_command::MappingType::TRUE_IF_INSTANCE,
        "The certificate should be a CA");
    command.addFlag("isSelfSigned", {"--self-signed"}, tempo_command::MappingType::TRUE_IF_INSTANCE,
        "The certificate should be self-signed");
    command.addHelpOption("help", {"-h", "--help"}, "Generate a key-pair");

    TU_RETURN_IF_NOT_OK (command.parse(argc - 1, &argv[1]));

    // determine the key type
    KeyType keyType;
    TU_RETURN_IF_NOT_OK (command.convert(keyType, keyTypeParser, "keyType"));

    // determine the digest id
    tempo_security::DigestId digestId;
    TU_RETURN_IF_NOT_OK (command.convert(digestId, digestIdParser, "digestId"));

    // determine the output directory
    std::filesystem::path outputDirectory;
    TU_RETURN_IF_NOT_OK (command.convert(outputDirectory, outputDirectoryParser, "outputDirectory"));

    // determine the signing certificate path
    std::filesystem::path signingCertificateFile;
    TU_RETURN_IF_NOT_OK (command.convert(signingCertificateFile, signingCertificateParser, "signingCertificate"));

    // determine the signing key path
    std::filesystem::path signingPrivateKeyFile;
    TU_RETURN_IF_NOT_OK (command.convert(signingPrivateKeyFile, signingPrivateKeyParser, "signingPrivateKey"));

    // determine the file name stem
    std::string fileName;
    TU_RETURN_IF_NOT_OK (command.convert(fileName, fileNameParser, "fileName"));

    // determine the organization
    std::string organization;
    TU_RETURN_IF_NOT_OK (command.convert(organization, organizationParser, "organization"));

    // determine the organizational unit
    std::string organizationalUnit;
    TU_RETURN_IF_NOT_OK (command.convert(organizationalUnit, organizationalUnitParser, "organizationalUnit"));

    // determine the common name
    std::string commonName;
    TU_RETURN_IF_NOT_OK (command.convert(commonName, commonNameParser, "commonName"));

    // determine the validity
    int validitySeconds;
    TU_RETURN_IF_NOT_OK (command.convert(validitySeconds, validitySecondsParser, "validitySeconds"));

    // determine the serial number
    int serialNumber;
    TU_RETURN_IF_NOT_OK (command.convert(serialNumber, serialNumberParser, "serialNumber"));

    // determine the serial number
    int pathlen;
    TU_RETURN_IF_NOT_OK (command.convert(pathlen, pathlenParser, "pathlen"));

    // determine if CA
    bool isCA;
    TU_RETURN_IF_NOT_OK (command.convert(isCA, isCAParser, "isCA"));

    // determine if self signed
    bool isSelfSigned;
    TU_RETURN_IF_NOT_OK (command.convert(isSelfSigned, isSelfSignedParser, "isSelfSigned"));

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
