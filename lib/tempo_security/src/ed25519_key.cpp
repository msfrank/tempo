
#include <openssl/pem.h>
#include <openssl/conf.h>

#include <tempo_security/ed25519_key.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/posix_result.h>

tempo_security::Ed25519Key::Ed25519Key(const std::filesystem::path &pemPrivateKeyFile)
    : m_pemPrivateKeyFile(pemPrivateKeyFile),
      m_key(nullptr)
{
    EVP_PKEY *key = nullptr;
    auto *bio = BIO_new_file(pemPrivateKeyFile.c_str(), "rx");
    if (bio == nullptr)
        goto err;
    if (!PEM_read_bio_PrivateKey(bio, &key, nullptr, nullptr))
        goto err;
    BIO_free(bio);
    m_status = {};
    m_key = key;
    return;

    err:
    if (bio != nullptr)
        BIO_free(bio);
    switch (errno) {
    case ENOENT:
        m_status = tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kFileNotFound,
            "private key file {} not found", pemPrivateKeyFile.string());
        return;
    case EACCES:
        m_status = tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kAccessDenied,
            "access denied for {}", pemPrivateKeyFile.string());
        return;
    default:
        m_status = tempo_utils::PosixStatus::fromError(errno);
        return;
    }
}

tempo_security::Ed25519Key::~Ed25519Key()
{
    EVP_PKEY_free(m_key);
}

bool
tempo_security::Ed25519Key::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_security::Ed25519Key::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_security::Ed25519Key::getPemPrivateKeyFile() const
{
    return m_pemPrivateKeyFile;
}

std::string
tempo_security::Ed25519Key::toString() const
{
    auto *bio = BIO_new(BIO_s_mem());
    EVP_PKEY_print_private(bio, m_key, 0, nullptr);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string s(data, len);
    BIO_free(bio);
    return s;
}
