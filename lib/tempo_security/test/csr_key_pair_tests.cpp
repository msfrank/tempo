#include "tempo_security/ed25519_private_key_generator.h"
#include "tempo_security/private_key.h"

#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>

#include <tempo_security/generate_utils.h>
#include <tempo_security/ecdsa_private_key_generator.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/security_types.h>
#include <tempo_security/x509_certificate_signing_request.h>
#include <tempo_security/x509_store.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::EcdsaPrivateKeyGenerator eccKeygen(tempo_security::CurveId::Prime256v1);
static tempo_security::Ed25519PrivateKeyGenerator ed25519Keygen;

class CSRKeyPair : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {

public:
    void SetUp() override
    {
        auto *keygen = GetParam();
        m_caKeyPair = tempo_security::GenerateUtils::generate_self_signed_ca_key_pair(
            *keygen,
            tempo_security::DigestId::None,
            "test_O",
            "test_OU",
            "caKeyPair",
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

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, CSRKeyPair, testing::Values(&rsaKeygen, &eccKeygen, &ed25519Keygen),
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

TEST_P(CSRKeyPair, TestGenerateCSRKeyPair)
{
    auto *keygen = GetParam();
    auto generateCsrResult = tempo_security::GenerateUtils::generate_csr_key_pair(
        *keygen,
        tempo_security::DigestId::None,
        "test_O",
        "test_OU",
        "csrKeyPair",
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateCsrResult.isResult());
    auto csrKeyPair = generateCsrResult.getResult();

    auto readRequestResult = tempo_security::X509CertificateSigningRequest::readFile(csrKeyPair.getPemRequestFile());
    ASSERT_TRUE (readRequestResult.isResult());
    auto req = readRequestResult.getResult();

    ASSERT_EQ ("test_O", req->getOrganization());
    ASSERT_EQ ("test_OU", req->getOrganizationalUnit());
    ASSERT_EQ ("csrKeyPair", req->getCommonName());

    auto readPrivateKeyResult = tempo_security::PrivateKey::readFile(
        csrKeyPair.getPemPrivateKeyFile());
    ASSERT_TRUE (readPrivateKeyResult.isResult());

    ASSERT_TRUE (std::filesystem::remove(csrKeyPair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(csrKeyPair.getPemRequestFile()));
}

TEST_P(CSRKeyPair, TestSignCertificateFromCSR)
{
    auto *keygen = GetParam();
    auto generateCsrResult = tempo_security::GenerateUtils::generate_csr_key_pair(
        *keygen,
        tempo_security::DigestId::None,
        "test_O",
        "test_OU",
        "csrKeyPair",
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateCsrResult.isResult());
    auto csrKeyPair = generateCsrResult.getResult();

    auto generateCertificateResult = tempo_security::generate_certificate_from_csr(
        csrKeyPair.getPemRequestFile(),
        getCAKeyPair(),
        tempo_security::DigestId::None,
        1,
        std::chrono::seconds{60},
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateCertificateResult.isResult());
    auto pemCertificateFile = generateCertificateResult.getResult();

    auto readFileResult = tempo_security::X509Certificate::readFile(pemCertificateFile);
    ASSERT_TRUE (readFileResult.isResult());
    auto cert = readFileResult.getResult();

    ASSERT_EQ ("test_O", cert->getOrganization());
    ASSERT_EQ ("test_OU", cert->getOrganizationalUnit());
    ASSERT_EQ ("csrKeyPair", cert->getCommonName());

    auto readPrivateKeyResult = tempo_security::PrivateKey::readFile(
        csrKeyPair.getPemPrivateKeyFile());
    ASSERT_TRUE (readPrivateKeyResult.isResult());

    tempo_security::X509StoreOptions options;
    auto loadStoreResult = tempo_security::X509Store::loadTrustedCerts(options, {getCAKeyPair().getPemCertificateFile()});
    ASSERT_TRUE (loadStoreResult.isResult());
    auto store = loadStoreResult.getResult();
    auto verifyStatus = store->verifyCertificate(pemCertificateFile);
    ASSERT_TRUE (verifyStatus.isOk());

    ASSERT_TRUE (std::filesystem::remove(csrKeyPair.getPemPrivateKeyFile()));
    ASSERT_TRUE (std::filesystem::remove(csrKeyPair.getPemRequestFile()));
    ASSERT_TRUE (std::filesystem::remove(pemCertificateFile));
}

TEST_P(CSRKeyPair, TestSignCertificateFromCSRFailsOnValidation)
{
    auto *keygen = GetParam();
    auto generateCsrResult = tempo_security::GenerateUtils::generate_csr_key_pair(
        *keygen,
        tempo_security::DigestId::None,
        "test_O",
        "test_OU",
        "csrKeyPair",
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateCsrResult.isResult());
    auto csrKeyPair = generateCsrResult.getResult();

    auto generateCertificateResult = tempo_security::generate_certificate_from_csr(
        csrKeyPair.getPemRequestFile(),
        getCAKeyPair(),
        tempo_security::DigestId::None,
        1,
        std::chrono::seconds{60},
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"),
        [](const tempo_security::CSRValidationParams &params) -> bool {
            return false;
        });

    ASSERT_TRUE (generateCertificateResult.isStatus());
}

TEST_P(CSRKeyPair, TestSignCertificateFromCSRPassesParams)
{
    auto *keygen = GetParam();
    auto generateCsrResult = tempo_security::GenerateUtils::generate_csr_key_pair(
        *keygen,
        tempo_security::DigestId::None,
        "test_O",
        "test_OU",
        "csrKeyPair",
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"));
    ASSERT_TRUE (generateCsrResult.isResult());
    auto csrKeyPair = generateCsrResult.getResult();

    std::string organization;
    std::string organizationalUnit;
    std::string commonName;

    auto generateCertificateResult = tempo_security::generate_certificate_from_csr(
        csrKeyPair.getPemRequestFile(),
        getCAKeyPair(),
        tempo_security::DigestId::None,
        1,
        std::chrono::seconds{60},
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_XXXXXXXX"),
        [&](const tempo_security::CSRValidationParams &params) -> bool {
            organization = params.organization;
            organizationalUnit = params.organizationalUnit;
            commonName = params.commonName;
            return true;
        });

    ASSERT_TRUE (generateCertificateResult.isResult());
    ASSERT_EQ ("test_O", organization);
    ASSERT_EQ ("test_OU", organizationalUnit);
    ASSERT_EQ ("csrKeyPair", commonName);
}
