#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>
#include <tempo_utils/log_console.h>
#include <tempo_utils/log_stream.h>

#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/ecc_private_key_generator.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/x509_certificate.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::ECCPrivateKeyGenerator eccKeygen(NID_X9_62_prime256v1);

class X509Certificate : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {

public:
    void SetUp() override
    {
        auto *keygen = GetParam();
        m_keyPair = tempo_security::generate_self_signed_key_pair(
            *keygen,
            "test_O",
            "test_OU",
            "ssKeyPair",
            1,
            std::chrono::seconds{60},
            std::filesystem::current_path(),
            tempo_utils::generate_name("test_key_XXXXXXXX")).orElseThrow();
        ASSERT_TRUE (m_keyPair.isValid());
    }
    void TearDown() override
    {
        ASSERT_TRUE(std::filesystem::remove(m_keyPair.getPemCertificateFile()));
        ASSERT_TRUE(std::filesystem::remove(m_keyPair.getPemPrivateKeyFile()));
    }
    tempo_security::CertificateKeyPair getKeyPair() const
    {
        return m_keyPair;
    }

private:
    tempo_security::CertificateKeyPair m_keyPair;
};

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, X509Certificate, testing::Values(&rsaKeygen, &eccKeygen),
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

TEST_P(X509Certificate, TestReadCertificate)
{
    auto keypair = getKeyPair();
    auto readFileResult = tempo_security::X509Certificate::readFile(keypair.getPemCertificateFile());
    ASSERT_TRUE (readFileResult.isResult());
    auto cert = readFileResult.getResult();
    TU_CONSOLE_OUT << "pemCertificateFile is " << keypair.getPemCertificateFile();
    ASSERT_TRUE (cert->isValid());
    ASSERT_TRUE (cert->getVersion() == 2);
    ASSERT_TRUE (cert->getSerialNumber() == 1);
    ASSERT_EQ (cert->getOrganization(), std::string("test_O"));
    ASSERT_EQ (cert->getOrganizationalUnit(), std::string("test_OU"));
    ASSERT_EQ (cert->getCommonName(), std::string("ssKeyPair"));
    ASSERT_FALSE (cert->isCertificateAuthority());
    TU_CONSOLE_OUT << cert->toString();
}

class X509CACertificate : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *>{

public:
    void SetUp() override
    {
        auto *keygen = GetParam();
        m_keyPair = tempo_security::generate_self_signed_ca_key_pair(
            *keygen,
            "test_O",
            "test_OU",
            "caKeyPair",
            1,
            std::chrono::seconds{60},
            -1,
            std::filesystem::current_path(),
            tempo_utils::generate_name("test_ca_key_XXXXXXXX")).orElseThrow();
        ASSERT_TRUE (m_keyPair.isValid());
    }
    void TearDown() override
    {
        ASSERT_TRUE(std::filesystem::remove(m_keyPair.getPemCertificateFile()));
        ASSERT_TRUE(std::filesystem::remove(m_keyPair.getPemPrivateKeyFile()));
    }
    tempo_security::CertificateKeyPair getKeyPair() const
    {
        return m_keyPair;
    }

private:
    tempo_security::CertificateKeyPair m_keyPair;
};

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, X509CACertificate, testing::Values(&rsaKeygen, &eccKeygen),
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

TEST_P(X509CACertificate, TestReadCACertificate)
{
    auto keypair = getKeyPair();

    auto readFileResult = tempo_security::X509Certificate::readFile(
        keypair.getPemCertificateFile());
    ASSERT_TRUE (readFileResult.isResult());
    auto cert = readFileResult.getResult();

    TU_CONSOLE_OUT << "pemCertificateFile is " << keypair.getPemCertificateFile();
    ASSERT_TRUE (cert->isValid());
    ASSERT_TRUE (cert->getVersion() == 2);
    ASSERT_TRUE (cert->getSerialNumber() == 1);
    ASSERT_EQ (cert->getOrganization(), std::string("test_O"));
    ASSERT_EQ (cert->getOrganizationalUnit(), std::string("test_OU"));
    ASSERT_EQ (cert->getCommonName(), std::string("caKeyPair"));
    ASSERT_TRUE (cert->isCertificateAuthority());
    TU_CONSOLE_OUT << cert->toString();
}