// #include <gtest/gtest.h>
//
// #include <tempo_utils/file_utilities.h>
// #include <tempo_utils/log_console.h>
// #include <tempo_utils/log_stream.h>
//
// #include <tempo_security/certificate_key_pair.h>
// #include <tempo_security/generate_utils.h>
// #include <tempo_security/ed25519_private_key_generator.h>
// #include <tempo_security/ed25519_key.h>
//
// class Ed25519Key : public ::testing::Test {
//
// public:
//     void SetUp() override
//     {
//         tempo_security::Ed25519PrivateKeyGenerator keygen;
//         m_edKeyPair = tempo_security::generate_self_signed_key_pair(
//             keygen,
//             "test_O",
//             "test_OU",
//             "edKeyPair",
//             1,
//             std::chrono::seconds{60},
//             std::filesystem::current_path(),
//             tempo_utils::generate_name("test_ed_key_XXXXXXXX")).orElseThrow();
//         ASSERT_TRUE (m_edKeyPair.isValid());
//     }
//     void TearDown() override
//     {
//         ASSERT_TRUE(std::filesystem::remove(m_edKeyPair.getPemCertificateFile()));
//         ASSERT_TRUE(std::filesystem::remove(m_edKeyPair.getPemPrivateKeyFile()));
//     }
//     tempo_security::CertificateKeyPair getEd25519KeyPair() const
//     {
//         return m_edKeyPair;
//     }
//
// private:
//     tempo_security::CertificateKeyPair m_edKeyPair;
// };
//
// TEST_F(Ed25519Key, TestReadEd25519Key)
// {
//     auto edKeyPair = getEd25519KeyPair();
//     tempo_security::Ed25519Key edKey(edKeyPair.getPemPrivateKeyFile());
//     ASSERT_TRUE (edKey.isValid());
//     TU_CONSOLE_OUT << "pemPrivateKeyFile is " << edKeyPair.getPemPrivateKeyFile();
//     TU_CONSOLE_OUT << edKey.toString();
// }
