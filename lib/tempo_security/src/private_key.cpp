
#include <openssl/pem.h>

#include <tempo_security/private_key.h>
#include <tempo_security/security_result.h>

tempo_security::PrivateKey::PrivateKey(EVP_PKEY *pkey)
    : m_pkey(pkey)
{
    TU_ASSERT (m_pkey != nullptr);
}

tempo_security::PrivateKey::~PrivateKey()
{
    EVP_PKEY_free(m_pkey);
}

tempo_security::KeyType
tempo_security::PrivateKey::getKeyType() const
{
    auto type = EVP_PKEY_type(EVP_PKEY_get_id(m_pkey));

    switch (type) {
        case EVP_PKEY_EC:
            return KeyType::Ecdsa;
        case EVP_PKEY_ED25519:
            return KeyType::Ed25519;
        case EVP_PKEY_RSA:
            return KeyType::Rsa;
        default:
            return KeyType::Invalid;
    }
}

EVP_PKEY *
tempo_security::PrivateKey::getPrivateKey() const
{
    return m_pkey;
}

std::string
tempo_security::PrivateKey::toPem() const
{
    auto *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(bio, m_pkey, nullptr, nullptr, 0, nullptr, nullptr);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string s(data, len);
    BIO_free(bio);
    return s;
}

std::string
tempo_security::PrivateKey::toString() const
{
    auto *bio = BIO_new(BIO_s_mem());
    EVP_PKEY_print_private(bio, m_pkey, 0, nullptr);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string s(data, len);
    BIO_free(bio);
    return s;
}

tempo_utils::Result<std::shared_ptr<tempo_security::PrivateKey>>
tempo_security::PrivateKey::readFile(const std::filesystem::path &pemPrivateKeyFile)
{
    EVP_PKEY *pkey = nullptr;
    auto *bio = BIO_new_file(pemPrivateKeyFile.c_str(), "rx");
    if (bio == nullptr) {
        switch (errno) {
            case ENOENT:
                return SecurityStatus::forCondition(SecurityCondition::kMissingPrivateKey,
                    "PEM private key file {} not found",
                    pemPrivateKeyFile.string());
            case EACCES:
                return SecurityStatus::forCondition(SecurityCondition::kMissingPrivateKey,
                    "access denied for {}",
                    pemPrivateKeyFile.string());
            default:
                return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
                    "failed to read PEM private key file {}",
                    pemPrivateKeyFile.string());
        }
    }

    if (!PEM_read_bio_PrivateKey(bio, &pkey, nullptr, nullptr)) {
        BIO_free(bio);
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to parse PEM private key");
    }
    BIO_free(bio);

    return std::shared_ptr<PrivateKey>(new PrivateKey(pkey));
}

tempo_utils::Result<std::shared_ptr<tempo_security::PrivateKey>>
tempo_security::PrivateKey::fromString(std::string_view pemPrivateKeyString)
{
    EVP_PKEY *pkey = nullptr;

    auto *bio = BIO_new_mem_buf(pemPrivateKeyString.data(), pemPrivateKeyString.size());
    if (bio == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to allocate BIO");

    if (!PEM_read_bio_PrivateKey(bio, &pkey, nullptr, nullptr)) {
        BIO_free(bio);
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to parse PEM private key");
    }
    BIO_free(bio);

    return std::shared_ptr<PrivateKey>(new PrivateKey(pkey));
}