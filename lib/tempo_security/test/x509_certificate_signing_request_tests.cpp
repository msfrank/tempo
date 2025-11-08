#include "tempo_security/ed25519_private_key_generator.h"

#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>
#include <tempo_utils/log_console.h>
#include <tempo_utils/log_stream.h>

#include <tempo_security/csr_key_pair.h>
#include <tempo_security/ecdsa_private_key_generator.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/security_types.h>
#include <tempo_security/x509_certificate_signing_request.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::EcdsaPrivateKeyGenerator eccKeygen(NID_X9_62_prime256v1);
static tempo_security::Ed25519PrivateKeyGenerator ed25519Keygen;

class X509CertificateSigningRequest : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {

public:
    void SetUp() override
    {
        auto *keygen = GetParam();
        m_csrKeyPair = tempo_security::GenerateUtils::generate_csr_key_pair(
            *keygen,
            tempo_security::DigestId::None,
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

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, X509CertificateSigningRequest, testing::Values(&rsaKeygen, &eccKeygen, &ed25519Keygen),
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

TEST_P(X509CertificateSigningRequest, TestReadCertificateSigningRequest)
{
    auto csrKeyPair = getCSRKeyPair();
    auto readFileResult = tempo_security::X509CertificateSigningRequest::readFile(csrKeyPair.getPemRequestFile());
    ASSERT_TRUE (readFileResult.isResult());
    auto req = readFileResult.getResult();
    TU_CONSOLE_OUT << "pemRequestFile is " << csrKeyPair.getPemRequestFile();
    ASSERT_EQ (req->getOrganization(), std::string("test_O"));
    ASSERT_EQ (req->getOrganizationalUnit(), std::string("test_OU"));
    ASSERT_EQ (req->getCommonName(), std::string("csrKeyPair"));
    TU_CONSOLE_OUT << req->toString();
}