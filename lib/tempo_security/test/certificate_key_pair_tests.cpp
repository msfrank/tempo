#include "tempo_security/ed25519_private_key_generator.h"
#include "tempo_security/private_key.h"

#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>

#include <tempo_security/generate_utils.h>
#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/ecdsa_private_key_generator.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/x509_certificate.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::EcdsaPrivateKeyGenerator eccKeygen(NID_X9_62_prime256v1);
static tempo_security::Ed25519PrivateKeyGenerator ed25519Keygen;

class CertificateKeyPair : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {
};

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, CertificateKeyPair, testing::Values(&rsaKeygen, &eccKeygen, &ed25519Keygen),
    [](const auto &info) {
        switch (info.param->getKeyType()) {
            case tempo_security::KeyType::Rsa:
                return "Rsa";
            case tempo_security::KeyType::Ecdsa:
                return "Ecdsa";
            case tempo_security::KeyType::Ed25519:
                return "Ed25519";
            default:
                return "???";
        }
    });

TEST_P(CertificateKeyPair, TestGenerateSelfSignedCertificateKeyPair)
{
    auto *keygen = GetParam();
    auto generateKeyPairResult = tempo_security::GenerateUtils::generate_self_signed_key_pair(
        *keygen,
        tempo_security::DigestId::None,
        "test_O",
        "test_OU",
        "ssKeyPair",
        1,
        std::chrono::seconds{60},
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateKeyPairResult.isResult());
    auto keypair = generateKeyPairResult.getResult();

    auto readCertificateResult = tempo_security::X509Certificate::readFile(
        keypair.getPemCertificateFile());
    ASSERT_TRUE (readCertificateResult.isResult());
    auto cert = readCertificateResult.getResult();

    ASSERT_EQ ("test_O", cert->getOrganization());
    ASSERT_EQ ("test_OU", cert->getOrganizationalUnit());
    ASSERT_EQ ("ssKeyPair", cert->getCommonName());

    auto readPrivateKeyResult = tempo_security::PrivateKey::readFile(
        keypair.getPemPrivateKeyFile());
    ASSERT_TRUE (readPrivateKeyResult.isResult());

    ASSERT_TRUE (std::filesystem::remove(keypair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(keypair.getPemCertificateFile()));
}

TEST_P(CertificateKeyPair, TestGenerateCACertificateKeyPair)
{
    auto *keygen = GetParam();
    auto generateKeyPairResult = tempo_security::GenerateUtils::generate_self_signed_ca_key_pair(
        *keygen,
        tempo_security::DigestId::None,
        "test_O",
        "test_OU",
        "caKeyPair",
        1,
        std::chrono::seconds{60},
        -1,
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateKeyPairResult.isResult());
    auto keypair = generateKeyPairResult.getResult();

    auto readCertificateResult = tempo_security::X509Certificate::readFile(
        keypair.getPemCertificateFile());
    ASSERT_TRUE (readCertificateResult.isResult());
    auto cert = readCertificateResult.getResult();

    ASSERT_EQ ("test_O", cert->getOrganization());
    ASSERT_EQ ("test_OU", cert->getOrganizationalUnit());
    ASSERT_EQ ("caKeyPair", cert->getCommonName());

    auto readPrivateKeyResult = tempo_security::PrivateKey::readFile(
        keypair.getPemPrivateKeyFile());
    ASSERT_TRUE (readPrivateKeyResult.isResult());

    ASSERT_TRUE (std::filesystem::remove(keypair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(keypair.getPemCertificateFile()));
}

TEST_P(CertificateKeyPair, TestGenerateCertificateKeyPairFromCA)
{
    auto *keygen = GetParam();
    auto generateKeyPairResult = tempo_security::GenerateUtils::generate_self_signed_ca_key_pair(
        *keygen,
        tempo_security::DigestId::None,
        "test_O",
        "test_OU",
        "keyPair",
        1,
        std::chrono::seconds{60},
        -1,
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateKeyPairResult.isResult());
    auto keypair = generateKeyPairResult.getResult();

    auto readCertificateResult = tempo_security::X509Certificate::readFile(
        keypair.getPemCertificateFile());
    ASSERT_TRUE (readCertificateResult.isResult());
    auto cert = readCertificateResult.getResult();

    ASSERT_EQ ("test_O", cert->getOrganization());
    ASSERT_EQ ("test_OU", cert->getOrganizationalUnit());
    ASSERT_EQ ("keyPair", cert->getCommonName());

    auto readPrivateKeyResult = tempo_security::PrivateKey::readFile(
        keypair.getPemPrivateKeyFile());
    ASSERT_TRUE (readPrivateKeyResult.isResult());

    ASSERT_TRUE (std::filesystem::remove(keypair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(keypair.getPemCertificateFile()));
}