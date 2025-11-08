// #include <gtest/gtest.h>
//
// #include <tempo_utils/file_utilities.h>
// #include <tempo_utils/log_console.h>
// #include <tempo_utils/log_stream.h>
//
// #include <tempo_security/certificate_key_pair.h>
// #include <tempo_security/generate_utils.h>
// #include <tempo_security/ecc_private_key_generator.h>
// #include <tempo_security/ecc_key.h>
//
// class ECCKey : public ::testing::Test {
//
// public:
//     void SetUp() override
//     {
//         tempo_security::ECCPrivateKeyGenerator keygen(tempo_security::ECCurveId::Prime256v1);
//         m_eccKeyPair = tempo_security::generate_self_signed_key_pair(
//             keygen,
//             "test_O",
//             "test_OU",
//             "ssKeyPair",
//             1,
//             std::chrono::seconds{60},
//             std::filesystem::current_path(),
//             tempo_utils::generate_name("test_ecc_key_XXXXXXXX")).orElseThrow();
//         ASSERT_TRUE (m_eccKeyPair.isValid());
//     }
//     void TearDown() override
//     {
//         ASSERT_TRUE(std::filesystem::remove(m_eccKeyPair.getPemCertificateFile()));
//         ASSERT_TRUE(std::filesystem::remove(m_eccKeyPair.getPemPrivateKeyFile()));
//     }
//     tempo_security::CertificateKeyPair getECCKeyPair() const
//     {
//         return m_eccKeyPair;
//     }
//
// private:
//     tempo_security::CertificateKeyPair m_eccKeyPair;
// };
//
// TEST_F(ECCKey, TestReadECCKey)
// {
//     auto eccKeyPair = getECCKeyPair();
//     tempo_security::ECCKey eccKey(eccKeyPair.getPemPrivateKeyFile());
//     ASSERT_TRUE (eccKey.isValid());
//     TU_CONSOLE_OUT << "pemPrivateKeyFile is " << eccKeyPair.getPemPrivateKeyFile();
//     TU_CONSOLE_OUT << eccKey.toString();
// }
