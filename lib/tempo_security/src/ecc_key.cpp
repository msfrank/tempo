
#include <openssl/pem.h>
#include <openssl/conf.h>

#include <tempo_security/ecc_key.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/posix_result.h>

tempo_security::ECCKey::ECCKey(const std::filesystem::path &pemPrivateKeyFile)
    : m_pemPrivateKeyFile(pemPrivateKeyFile),
      m_ecc(nullptr)
{
    EC_KEY *ecc = nullptr;
    auto *bio = BIO_new_file(pemPrivateKeyFile.c_str(), "rx");
    if (bio == nullptr)
        goto err;
    if (!PEM_read_bio_ECPrivateKey(bio, &ecc, nullptr, nullptr))
        goto err;
    BIO_free(bio);
    m_status = {};
    m_ecc = ecc;
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

tempo_security::ECCKey::~ECCKey()
{
    EC_KEY_free(m_ecc);
}

bool
tempo_security::ECCKey::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_security::ECCKey::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_security::ECCKey::getPemPrivateKeyFile() const
{
    return m_pemPrivateKeyFile;
}

std::string
tempo_security::ECCKey::toString() const
{
    auto *bio = BIO_new(BIO_s_mem());
    EC_KEY_print(bio, m_ecc, 0);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string s(data, len);
    BIO_free(bio);
    return s;
}
