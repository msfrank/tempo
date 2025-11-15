
#include "tempo_utils/file_reader.h"

#include <gtest/gtest.h>

#include <tempo_utils/file_utilities.h>
#include <tempo_utils/log_console.h>
#include <tempo_utils/log_stream.h>

#include <tempo_security/certificate_key_pair.h>
#include <tempo_security/ecdsa_private_key_generator.h>
#include <tempo_security/ed25519_private_key_generator.h>
#include <tempo_security/generate_utils.h>
#include <tempo_security/private_key.h>
#include <tempo_security/rsa_private_key_generator.h>

static tempo_security::RSAPrivateKeyGenerator rsaKeygen(tempo_security::kRSAKeyBits, tempo_security::kRSAPublicExponent);
static tempo_security::EcdsaPrivateKeyGenerator eccKeygen(tempo_security::CurveId::Prime256v1);
static tempo_security::Ed25519PrivateKeyGenerator ed25519Keygen;

class PrivateKey : public testing::TestWithParam<const tempo_security::AbstractPrivateKeyGenerator *> {

public:
    void SetUp() override
    {
        auto *keygen = GetParam();
        m_keyPair = tempo_security::GenerateUtils::generate_self_signed_key_pair(
            *keygen,
            tempo_security::DigestId::None,
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

INSTANTIATE_TEST_SUITE_P(PrivateKeyGenerators, PrivateKey, testing::Values(&rsaKeygen, &eccKeygen, &ed25519Keygen),
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

TEST_P(PrivateKey, TestReadPrivateKey)
{
    auto keypair = getKeyPair();
    auto readFileResult = tempo_security::PrivateKey::readFile(keypair.getPemPrivateKeyFile());
    ASSERT_TRUE (readFileResult.isResult());
    auto privateKey = readFileResult.getResult();
    TU_CONSOLE_OUT << "pemPrivateKeyFile is " << keypair.getPemPrivateKeyFile();
    auto *keygen = GetParam();
    ASSERT_EQ (keygen->getKeyType(), privateKey->getKeyType());
    TU_CONSOLE_OUT << privateKey->toString();
}

TEST_P(PrivateKey, TestPrivateKeyToPem)
{
    auto keypair = getKeyPair();

    tempo_utils::FileReader privateKeyReader(keypair.getPemPrivateKeyFile());
    ASSERT_TRUE (privateKeyReader.isValid());
    auto privateKeyBytes = privateKeyReader.getBytes();

    auto fromStringResult = tempo_security::PrivateKey::fromString(privateKeyBytes->getStringView());
    ASSERT_TRUE (fromStringResult.isResult());
    auto privateKey = fromStringResult.getResult();

    ASSERT_EQ (privateKeyBytes->getStringView(), privateKey->toPem());
}
