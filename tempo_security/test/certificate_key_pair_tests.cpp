#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>

#include <tempo_security/generate_utils.h>
#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/ecc_private_key_generator.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/x509_certificate.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::ECCPrivateKeyGenerator eccKeygen(NID_X9_62_prime256v1);

class CertificateKeyPair : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {
};

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, CertificateKeyPair, testing::Values(&rsaKeygen, &eccKeygen),
    [](const auto &info) {
        switch (info.param->getKeyType()) {
            case tempo_security::KeyType::RSA:
                return "RSA";
            case tempo_security::KeyType::ECC:
                return "ECC";
            default:
                return "???";
        }
    });

TEST_P(CertificateKeyPair, TestGenerateSelfSignedCertificateKeyPair)
{
    auto *keygen = GetParam();
    auto generateKeyPairResult = tempo_security::generate_self_signed_key_pair(
        *keygen,
        "test_O",
        "test_OU",
        "ssKeyPair",
        1,
        std::chrono::seconds{60},
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateKeyPairResult.isResult());
    auto keypair = generateKeyPairResult.getResult();

    auto readFileResult = tempo_security::X509Certificate::readFile(keypair.getPemCertificateFile());
    ASSERT_TRUE (readFileResult.isResult());
    auto cert = readFileResult.getResult();

    ASSERT_TRUE (cert->isValid());
    ASSERT_EQ ("test_O", cert->getOrganization());
    ASSERT_EQ ("test_OU", cert->getOrganizationalUnit());
    ASSERT_EQ ("ssKeyPair", cert->getCommonName());

    ASSERT_TRUE (keygen->isValidPrivateKey(keypair.getPemPrivateKeyFile()));

    ASSERT_TRUE (std::filesystem::remove(keypair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(keypair.getPemCertificateFile()));
}

TEST_P(CertificateKeyPair, TestGenerateCACertificateKeyPair)
{
    auto *keygen = GetParam();
    auto generateKeyPairResult = tempo_security::generate_self_signed_ca_key_pair(
        *keygen,
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

    auto readFileResult = tempo_security::X509Certificate::readFile(
        keypair.getPemCertificateFile());
    ASSERT_TRUE (readFileResult.isResult());
    auto cert = readFileResult.getResult();

    ASSERT_TRUE (cert->isValid());
    ASSERT_EQ ("test_O", cert->getOrganization());
    ASSERT_EQ ("test_OU", cert->getOrganizationalUnit());
    ASSERT_EQ ("caKeyPair", cert->getCommonName());

    ASSERT_TRUE (keygen->isValidPrivateKey(keypair.getPemPrivateKeyFile()));

    ASSERT_TRUE (std::filesystem::remove(keypair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(keypair.getPemCertificateFile()));
}

TEST_P(CertificateKeyPair, TestGenerateCertificateKeyPairFromCA)
{
    auto *keygen = GetParam();
    auto generateKeyPairResult = tempo_security::generate_self_signed_ca_key_pair(
        *keygen,
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

    auto readFileResult = tempo_security::X509Certificate::readFile(
        keypair.getPemCertificateFile());
    ASSERT_TRUE (readFileResult.isResult());
    auto cert = readFileResult.getResult();

    ASSERT_TRUE (cert->isValid());
    ASSERT_EQ ("test_O", cert->getOrganization());
    ASSERT_EQ ("test_OU", cert->getOrganizationalUnit());
    ASSERT_EQ ("keyPair", cert->getCommonName());

    ASSERT_TRUE (keygen->isValidPrivateKey(keypair.getPemPrivateKeyFile()));

    ASSERT_TRUE (std::filesystem::remove(keypair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(keypair.getPemCertificateFile()));
}