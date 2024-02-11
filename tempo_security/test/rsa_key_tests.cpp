#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>
#include <tempo_utils/log_console.h>
#include <tempo_utils/log_stream.h>

#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/rsa_private_key_generator.h>
#include <tempo_security/rsa_key.h>

class RSAKey : public ::testing::Test {

public:
    void SetUp() override
    {
        tempo_security::RSAPrivateKeyGenerator keygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
        m_rsaKeyPair = tempo_security::generate_self_signed_key_pair(
            keygen,
            "test_O",
            "test_OU",
            "test_CN",
            1,
            std::chrono::seconds{60},
            std::filesystem::current_path(),
            tempo_utils::generate_name("test_rsa_key_XXXXXXXX")).orElseThrow();
        ASSERT_TRUE (m_rsaKeyPair.isValid());
    }
    void TearDown() override
    {
        ASSERT_TRUE(std::filesystem::remove(m_rsaKeyPair.getPemCertificateFile()));
        ASSERT_TRUE(std::filesystem::remove(m_rsaKeyPair.getPemPrivateKeyFile()));
    }
    tempo_security::CertificateKeyPair getRSAKeyPair() const
    {
        return m_rsaKeyPair;
    }

private:
    tempo_security::CertificateKeyPair m_rsaKeyPair;
};

TEST_F(RSAKey, TestReadRSAKey)
{
    auto rsaKeyPair = getRSAKeyPair();
    tempo_security::RSAKey rsaKey(rsaKeyPair.getPemPrivateKeyFile());
    ASSERT_TRUE (rsaKey.isValid());
    TU_CONSOLE_OUT << "pemPrivateKeyFile is " << rsaKeyPair.getPemPrivateKeyFile();
    TU_CONSOLE_OUT << rsaKey.toString();
}