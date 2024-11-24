
#include <openssl/evp.h>

#include <tempo_security/sha256_hash.h>
#include <tempo_utils/log_message.h>

tempo_security::Sha256Hash::Sha256Hash()
{
    m_ctx = EVP_MD_CTX_new();
    m_dirty = true;
}

tempo_security::Sha256Hash::~Sha256Hash()
{
    EVP_MD_CTX_free((EVP_MD_CTX *) m_ctx);
}

bool
tempo_security::Sha256Hash::addData(std::string_view data)
{
    auto *ctx = (EVP_MD_CTX *) m_ctx;
    if (m_dirty) {
        if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) == 0)
            return false;
        m_dirty = false;
    }
    if (EVP_DigestUpdate(ctx, data.data(), data.size()) == 0)
        return false;
    return true;
}

std::string
tempo_security::Sha256Hash::getResult()
{
    if (m_dirty)
        return {};

    auto *ctx = (EVP_MD_CTX *) m_ctx;
    m_dirty = true;
    std::string result;
    result.resize(EVP_MAX_MD_SIZE);
    unsigned int md_len;
    EVP_DigestFinal_ex(ctx, (unsigned char *) result.data(), &md_len);
    result.resize(md_len);
    return result;
}

std::string
tempo_security::Sha256Hash::hash(std::string_view data)
{
    tempo_security::Sha256Hash hasher;
    hasher.addData(data);
    return hasher.getResult();
}
