
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <tempo_security/csr_key_pair.h>
#include <tempo_security/security_types.h>
#include <tempo_utils/log_message.h>

tempo_security::CSRKeyPair::CSRKeyPair()
{
}

tempo_security::CSRKeyPair::CSRKeyPair(
    KeyType keyType,
    const std::filesystem::path &pemPrivateKeyFile,
    const std::filesystem::path &pemRequestFile)
    : m_keyType(keyType),
      m_pemPrivateKeyFile(pemPrivateKeyFile),
      m_pemRequestFile(pemRequestFile)
{
    TU_ASSERT (m_keyType != KeyType::INVALID);
    TU_ASSERT (!m_pemPrivateKeyFile.empty());
    TU_ASSERT (!m_pemRequestFile.empty());
}

tempo_security::CSRKeyPair::CSRKeyPair(const CSRKeyPair &other)
    : m_keyType(other.m_keyType),
      m_pemPrivateKeyFile(other.m_pemPrivateKeyFile),
      m_pemRequestFile(other.m_pemRequestFile)
{
}

bool
tempo_security::CSRKeyPair::isValid() const
{
    return m_keyType != KeyType::INVALID;
}

tempo_security::KeyType
tempo_security::CSRKeyPair::getKeyType() const
{
    return m_keyType;
}

std::filesystem::path
tempo_security::CSRKeyPair::getPemPrivateKeyFile() const
{
    return m_pemPrivateKeyFile;
}

std::filesystem::path
tempo_security::CSRKeyPair::getPemRequestFile() const
{
    return m_pemRequestFile;
}

struct LoadCSRKeyPairCtx {
    X509_REQ *req = nullptr;
    EVP_PKEY *key = nullptr;

    ~LoadCSRKeyPairCtx() {
        if (req)
            X509_REQ_free(req);
        if (key)
            EVP_PKEY_free(key);
    }
};

tempo_utils::Result<tempo_security::CSRKeyPair>
tempo_security::CSRKeyPair::load(
    const std::filesystem::path &pemPrivateKeyFile,
    const std::filesystem::path &pemRequestFile)
{
    LoadCSRKeyPairCtx ctx;

    // load the CA cert
    auto *req_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(req_bio, pemRequestFile.c_str())) {
        ctx.req = PEM_read_bio_X509_REQ(req_bio, NULL, NULL, NULL);
        BIO_free_all(req_bio);
        if (ctx.req == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid certificate signing request {}", pemRequestFile.string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingCertificate,
            "missing certificate signing request {}", pemRequestFile.string());
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
    if (!X509_REQ_check_private_key(ctx.req, ctx.key))
        return SecurityStatus::forCondition(SecurityCondition::kParseError,
            "private key {} does not match certificate signing request {}",
            pemPrivateKeyFile.string(), pemRequestFile.string());

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

    return CSRKeyPair(keyType, pemPrivateKeyFile, pemRequestFile);
}
