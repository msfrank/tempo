#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>
#include <tempo_utils/log_console.h>
#include <tempo_utils/log_stream.h>

#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/ecc_private_key_generator.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/x509_store.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::ECCPrivateKeyGenerator eccKeygen(NID_X9_62_prime256v1);

class X509Store : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {

public:
    void SetUp() override
    {
        auto *keygen = GetParam();
        m_caKeyPair = tempo_security::generate_self_signed_ca_key_pair(
            *keygen,
            "test_O",
            "test_OU",
            "caKeyPair",
            1,
            std::chrono::seconds{3600},
            -1,
            std::filesystem::current_path(),
            tempo_utils::generate_name("test_ca_key_XXXXXXXX")).orElseThrow();
        ASSERT_TRUE (m_caKeyPair.isValid());
        m_keyPair = tempo_security::generate_key_pair(
            m_caKeyPair,
            *keygen,
            "test_O",
            "test_OU",
            "keypair",
            2,
            std::chrono::seconds{3600},
            std::filesystem::current_path(),
            tempo_utils::generate_name("test_key_XXXXXXXX")).orElseThrow();
        ASSERT_TRUE (m_keyPair.isValid());
    }
    void TearDown() override
    {
        ASSERT_TRUE(std::filesystem::remove(m_caKeyPair.getPemCertificateFile()));
        ASSERT_TRUE(std::filesystem::remove(m_caKeyPair.getPemPrivateKeyFile()));
        ASSERT_TRUE(std::filesystem::remove(m_keyPair.getPemCertificateFile()));
        ASSERT_TRUE(std::filesystem::remove(m_keyPair.getPemPrivateKeyFile()));
    }
    tempo_security::CertificateKeyPair getCAKeyPair() const
    {
        return m_caKeyPair;
    }
    tempo_security::CertificateKeyPair getKeyPair() const
    {
        return m_keyPair;
    }
private:
    tempo_security::CertificateKeyPair m_keyPair;
    tempo_security::CertificateKeyPair m_caKeyPair;
};

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, X509Store, testing::Values(&rsaKeygen, &eccKeygen),
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

TEST_P(X509Store, TestStoreContainsCertificateFromCAFileLocation)
{
    auto caKeyPair = getCAKeyPair();

    tempo_security::X509StoreOptions options;
    auto loadStoreResult = tempo_security::X509Store::loadLocations(
        options, {}, caKeyPair.getPemCertificateFile());
    ASSERT_TRUE (loadStoreResult.isResult());
    auto store = loadStoreResult.getResult();

    TU_CONSOLE_OUT << "store contains the following certs:";
    auto certificatesList = store->listCertificates();
    for (int i = 0; i < static_cast<int>(certificatesList.size()); i++) {
        const auto &certificate = certificatesList.at(i);
        TU_CONSOLE_OUT << "    " << i << ": cn=" << certificate->getCommonName()
            << " notValidBefore=" << certificate->getNotValidBefore().time_since_epoch().count()
            << " notValidAfter=" << certificate->getNotValidAfter().time_since_epoch().count()
        ;
    }

    ASSERT_EQ (1, certificatesList.size());

    auto loadCACertResult = tempo_security::X509Certificate::readFile(caKeyPair.getPemCertificateFile());
    ASSERT_TRUE (loadCACertResult.isResult());
    ASSERT_EQ (*loadCACertResult.getResult(), *certificatesList.at(0));
}

TEST_P(X509Store, TestVerifyCertificateAgainstLoadedTrustedCerts)
{
    auto caKeyPair = getCAKeyPair();
    auto keypair = getKeyPair();

    tempo_security::X509StoreOptions options;
     auto loadStoreResult = tempo_security::X509Store::loadTrustedCerts(
         options, {caKeyPair.getPemCertificateFile()});
    ASSERT_TRUE (loadStoreResult.isResult());
    auto store = loadStoreResult.getResult();

    auto status = store->verifyCertificate(keypair.getPemCertificateFile());
    TU_CONSOLE_OUT << "verify status: " << status;
    ASSERT_TRUE (status.isOk());
}

TEST_P(X509Store, TestVerifyCertificateAgainstLoadedLocationCACertsFile)
{
    auto caKeyPair = getCAKeyPair();
    auto keypair = getKeyPair();

    tempo_security::X509StoreOptions options;
    auto loadStoreResult = tempo_security::X509Store::loadLocations(
        options, {}, caKeyPair.getPemCertificateFile());
    ASSERT_TRUE (loadStoreResult.isResult());
    auto store = loadStoreResult.getResult();

    auto status = store->verifyCertificate(keypair.getPemCertificateFile());
    TU_CONSOLE_OUT << "verify status: " << status;
    ASSERT_TRUE (status.isOk());
}

TEST_P(X509Store, TestVerifySelfSignedCertificateFails)
{
    auto *keygen = GetParam();
    auto caKeyPair = getCAKeyPair();
    auto generateKeyPairResult = tempo_security::generate_self_signed_key_pair(
        *keygen,
        "test_O",
        "test_OU",
        "selfSignedKeyPair",
        1,
        std::chrono::seconds{60},
        std::filesystem::current_path(),
        tempo_utils::generate_name("test_self_signed_key_XXXXXXXX"));
    TU_ASSERT (generateKeyPairResult.isResult());
    auto selfSignedKeyPair = generateKeyPairResult.getResult();

    tempo_security::X509StoreOptions options;
    auto loadStoreResult = tempo_security::X509Store::loadTrustedCerts(
        options, {caKeyPair.getPemCertificateFile()});
    ASSERT_TRUE (loadStoreResult.isResult());
    auto store = loadStoreResult.getResult();

    auto status = store->verifyCertificate(selfSignedKeyPair.getPemCertificateFile());
    TU_CONSOLE_OUT << "verify status: " << status;
    ASSERT_TRUE (std::filesystem::remove(selfSignedKeyPair.getPemCertificateFile()));
    ASSERT_TRUE (std::filesystem::remove(selfSignedKeyPair.getPemPrivateKeyFile()));
    ASSERT_FALSE (status.isOk());

    tempo_security::SecurityStatus securityStatus;
    ASSERT_TRUE (status.convertTo(securityStatus));
    ASSERT_EQ (tempo_security::SecurityCondition::kVerificationFailure, securityStatus.getCondition());
}
