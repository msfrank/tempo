#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>

#include <tempo_security/generate_utils.h>
#include <tempo_security/ecc_private_key_generator.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/security_types.h>
#include <tempo_security/x509_certificate_signing_request.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::ECCPrivateKeyGenerator eccKeygen(NID_X9_62_prime256v1);

class CSRKeyPair : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {

public:
    void SetUp() override
    {
        auto *keygen = GetParam();
        m_caKeyPair = tempo_security::generate_self_signed_ca_key_pair(
            *keygen,
            "test_O",
            "test_OU",
            "test_CN",
            1,
            std::chrono::seconds{60},
            -1,
            std::filesystem::current_path(),
            tempo_utils::generate_name("test_csr_key_pair_CA_XXXXXXXX")).orElseThrow();
        ASSERT_TRUE (m_caKeyPair.isValid());
    }
    void TearDown() override
    {
        ASSERT_TRUE(std::filesystem::remove(m_caKeyPair.getPemCertificateFile()));
        ASSERT_TRUE(std::filesystem::remove(m_caKeyPair.getPemPrivateKeyFile()));
    }
    tempo_security::CertificateKeyPair getCAKeyPair() const
    {
        return m_caKeyPair;
    }

private:
    tempo_security::CertificateKeyPair m_caKeyPair;
};

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, CSRKeyPair, testing::Values(&rsaKeygen, &eccKeygen),
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

TEST_P(CSRKeyPair, TestGenerateSelfSignedCSRKeyPair)
{
    auto *keygen = GetParam();
    auto generateCsrResult = tempo_security::generate_csr_key_pair(
        *keygen,
        "test_O",
        "test_OU",
        "test_CN",
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateCsrResult.isResult());
    auto csrKeyPair = generateCsrResult.getResult();

    auto generateCertificateResult = tempo_security::generate_certificate_from_csr(
        csrKeyPair.getPemRequestFile(),
        getCAKeyPair(),
        1,
        std::chrono::seconds{60},
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateCertificateResult.isResult());
    auto pemCertificateFile = generateCertificateResult.getResult();

    ASSERT_TRUE (std::filesystem::remove(csrKeyPair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(csrKeyPair.getPemRequestFile()));

    auto readFileResult = tempo_security::X509Certificate::readFile(pemCertificateFile);
    ASSERT_TRUE (readFileResult.isResult());
    auto cert = readFileResult.getResult();
    ASSERT_TRUE (cert->isValid());
}