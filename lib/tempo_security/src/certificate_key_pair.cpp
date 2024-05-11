
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <tempo_security/certificate_key_pair.h>
#include <tempo_utils/log_message.h>

tempo_security::CertificateKeyPair::CertificateKeyPair()
{
}

tempo_security::CertificateKeyPair::CertificateKeyPair(
    KeyType keyType,
    const std::filesystem::path &pemPrivateKeyFile,
    const std::filesystem::path &pemCertificateFile)
    : m_keyType(keyType),
      m_pemPrivateKeyFile(pemPrivateKeyFile),
      m_pemCertificateFile(pemCertificateFile)
{
    TU_ASSERT (m_keyType != KeyType::INVALID);
    TU_ASSERT (!m_pemPrivateKeyFile.empty());
    TU_ASSERT (!m_pemCertificateFile.empty());
}

tempo_security::CertificateKeyPair::CertificateKeyPair(const CertificateKeyPair &other)
    : m_keyType(other.m_keyType),
      m_pemPrivateKeyFile(other.m_pemPrivateKeyFile),
      m_pemCertificateFile(other.m_pemCertificateFile)
{
}

bool
tempo_security::CertificateKeyPair::isValid() const
{
    return m_keyType != KeyType::INVALID;
}

tempo_security::KeyType
tempo_security::CertificateKeyPair::getKeyType() const
{
    return m_keyType;
}

std::filesystem::path
tempo_security::CertificateKeyPair::getPemPrivateKeyFile() const
{
    return m_pemPrivateKeyFile;
}

std::filesystem::path
tempo_security::CertificateKeyPair::getPemCertificateFile() const
{
    return m_pemCertificateFile;
}

struct LoadCertificateKeyPairCtx {
    X509 *crt = nullptr;
    EVP_PKEY *key = nullptr;

    ~LoadCertificateKeyPairCtx() {
        if (crt)
            X509_free(crt);
        if (key)
            EVP_PKEY_free(key);
    }
};

tempo_utils::Result<tempo_security::CertificateKeyPair>
tempo_security::CertificateKeyPair::load(
    const std::filesystem::path &pemPrivateKeyFile,
    const std::filesystem::path &pemCertificateFile)
{
    LoadCertificateKeyPairCtx ctx;

    // load the CA cert
    auto *crt_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(crt_bio, pemCertificateFile.c_str())) {
        ctx.crt = PEM_read_bio_X509(crt_bio, NULL, NULL, NULL);
        BIO_free_all(crt_bio);
        if (ctx.crt == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid certificate {}", pemCertificateFile.string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingCertificate,
            "missing certificate {}", pemCertificateFile.string());
    }

    // load the CA private key
    auto *key_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(key_bio, pemPrivateKeyFile.c_str())) {
        ctx.key = PEM_read_bio_PrivateKey(key_bio, NULL, NULL, NULL);
        BIO_free_all(key_bio);
        if (ctx.key == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid private key {}", pemPrivateKeyFile.string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingPrivateKey,
            "missing private key {}", pemPrivateKeyFile.string());
    }

    // verify the private key matches the certificate
    if (!X509_check_private_key(ctx.crt, ctx.key))
        return SecurityStatus::forCondition(SecurityCondition::kParseError,
            "private key {} does not match certificate {}",
                pemPrivateKeyFile.string(), pemCertificateFile.string());

    // detect the key type
    KeyType keyType;
    switch (EVP_PKEY_base_id(ctx.key)) {
        case EVP_PKEY_RSA:
            keyType = KeyType::RSA;
            break;
        case EVP_PKEY_EC:
            keyType = KeyType::ECC;
            break;
        default:
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "unknown key type for private key {}", pemPrivateKeyFile.string());
    }

    return CertificateKeyPair(keyType, pemPrivateKeyFile, pemCertificateFile);
}