#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>
#include <tempo_utils/log_console.h>
#include <tempo_utils/log_stream.h>

#include <tempo_security/csr_key_pair.h>
#include <tempo_security/ecc_private_key_generator.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/security_types.h>
#include <tempo_security/x509_certificate_signing_request.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::ECCPrivateKeyGenerator eccKeygen(NID_X9_62_prime256v1);

class X509CertificateSigningRequest : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {

public:
    void SetUp() override
    {
        auto *keygen = GetParam();
        m_csrKeyPair = tempo_security::generate_csr_key_pair(
            *keygen,
            "test_O",
            "test_OU",
            "csrKeyPair",
            std::filesystem::current_path(),
            tempo_utils::generate_name("test_key_XXXXXXXX")).orElseThrow();
        ASSERT_TRUE (m_csrKeyPair.isValid());
    }
    void TearDown() override
    {
        ASSERT_TRUE(std::filesystem::remove(m_csrKeyPair.getPemRequestFile()));
        ASSERT_TRUE(std::filesystem::remove(m_csrKeyPair.getPemPrivateKeyFile()));
    }
    tempo_security::CSRKeyPair getCSRKeyPair() const
    {
        return m_csrKeyPair;
    }

private:
    tempo_security::CSRKeyPair m_csrKeyPair;
};

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, X509CertificateSigningRequest, testing::Values(&rsaKeygen, &eccKeygen),
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

TEST_P(X509CertificateSigningRequest, TestReadCertificateSigningRequest)
{
    auto csrKeyPair = getCSRKeyPair();
    auto readFileResult = tempo_security::X509CertificateSigningRequest::readFile(csrKeyPair.getPemRequestFile());
    ASSERT_TRUE (readFileResult.isResult());
    auto req = readFileResult.getResult();
    TU_CONSOLE_OUT << "pemRequestFile is " << csrKeyPair.getPemRequestFile();
    ASSERT_TRUE (req->isValid());
    ASSERT_EQ (req->getOrganization(), std::string("test_O"));
    ASSERT_EQ (req->getOrganizationalUnit(), std::string("test_OU"));
    ASSERT_EQ (req->getCommonName(), std::string("csrKeyPair"));
    TU_CONSOLE_OUT << req->toString();
}