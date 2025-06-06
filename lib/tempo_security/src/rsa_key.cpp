
#include <openssl/pem.h>
#include <openssl/conf.h>

#include <tempo_security/rsa_key.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/posix_result.h>

tempo_security::RSAKey::RSAKey(const std::filesystem::path &pemPrivateKeyFile)
    : m_pemPrivateKeyFile(pemPrivateKeyFile),
      m_rsa(nullptr)
{
    RSA *rsa = nullptr;
    auto *bio = BIO_new_file(pemPrivateKeyFile.c_str(), "rx");
    if (bio == nullptr)
        goto err;
    if (!PEM_read_bio_RSAPrivateKey(bio, &rsa, nullptr, nullptr))
        goto err;
    BIO_free(bio);
    m_status = {};
    m_rsa = rsa;
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

tempo_security::RSAKey::~RSAKey()
{
    RSA_free(m_rsa);
}

bool
tempo_security::RSAKey::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_security::RSAKey::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_security::RSAKey::getPemPrivateKeyFile() const
{
    return m_pemPrivateKeyFile;
}

std::string
tempo_security::RSAKey::toString() const
{
    auto *bio = BIO_new(BIO_s_mem());
    RSA_print(bio, m_rsa, 0);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string s(data, len);
    BIO_free(bio);
    return s;
}