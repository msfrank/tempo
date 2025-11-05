#include "tempo_security/x509_certificate.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_utils/file_utilities.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/memory_bytes.h>

#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/digest_utils.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/ed25519_key.h>
#include <tempo_security/ed25519_private_key_generator.h>

class Digest : public ::testing::Test {

public:
    void SetUp() override
    {
        tempo_security::Ed25519PrivateKeyGenerator keygen;
        m_keyPair = tempo_security::generate_self_signed_key_pair(
            keygen,
            "test_O",
            "test_OU",
            "edKeyPair",
            1,
            std::chrono::seconds{3600},
            std::filesystem::current_path(),
            tempo_utils::generate_name("test_ed_key_XXXXXXXX")).orElseThrow();
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

TEST_F(Digest, SignAndVerify)
{
    auto keyPair = getKeyPair();
    auto content = tempo_utils::MemoryBytes::copy("hello, world!");

    auto pemPrivateKeyFile = keyPair.getPemPrivateKeyFile();
    TU_CONSOLE_OUT << "private key: " << pemPrivateKeyFile;
    tempo_security::Ed25519Key privateKey(pemPrivateKeyFile);
    TU_CONSOLE_OUT << privateKey.toString();

    auto pemCertificateFile = keyPair.getPemCertificateFile();
    TU_CONSOLE_OUT << "certificate: " << pemCertificateFile;
    std::shared_ptr<tempo_security::X509Certificate> certificate;
    TU_ASSIGN_OR_RAISE (certificate, tempo_security::X509Certificate::readFile(pemCertificateFile));
    TU_CONSOLE_OUT << certificate->toString();

    auto generateSignatureResult = tempo_security::generate_signed_message_digest(
        content->getSpan(), pemPrivateKeyFile,
        tempo_security::DigestId::None);
    ASSERT_TRUE (generateSignatureResult.isResult());
    auto digest = generateSignatureResult.getResult();

    auto verifySignatureResult = tempo_security::verify_signed_message_digest(
        content->getSpan(), digest, pemCertificateFile,
        tempo_security::DigestId::None);
    ASSERT_TRUE (verifySignatureResult.isResult()) << "expected result, got: " << verifySignatureResult.getStatus().toString();

    bool success = verifySignatureResult.getResult();
    ASSERT_TRUE (success);
}
