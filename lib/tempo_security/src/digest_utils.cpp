
#include "tempo_security/security_result.h"

#include <openssl/decoder.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <tempo_security/digest_utils.h>

tempo_security::Digest::Digest(std::vector<tu_uint8> &&data)
{
    m_data = std::make_shared<std::vector<tu_uint8>>(std::move(data));
}

tempo_security::Digest::Digest(std::span<const tu_uint8> data)
{
    m_data = std::make_shared<std::vector<tu_uint8>>(data.begin(), data.end());
}

tempo_security::Digest::Digest(const Digest &other)
    : m_data(other.m_data)
{
}

bool
tempo_security::Digest::isValid() const
{
    return m_data != nullptr;
}

const tu_uint8 *
tempo_security::Digest::getData() const
{
    if (m_data == nullptr)
        return nullptr;
    return m_data->data();
}

size_t
tempo_security::Digest::getSize() const
{
    if (m_data == nullptr)
        return 0;
    return m_data->size();
}

std::span<const tu_uint8>
tempo_security::Digest::getSpan() const
{
    if (m_data == nullptr)
        return {};
    return std::span(m_data->data(), m_data->size());
}


/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct GenerateSignedDigestCtx {
    EVP_MD_CTX *mdctx = nullptr;

    ~GenerateSignedDigestCtx() {
        if (mdctx)
            EVP_MD_CTX_free(mdctx);
    }
};

tempo_utils::Result<tempo_security::Digest>
tempo_security::DigestUtils::generate_signed_message_digest(
    std::span<const tu_uint8> data,
    std::shared_ptr<PrivateKey> privateKey,
    DigestId digestId)
{
    GenerateSignedDigestCtx ctx;
    auto *key = privateKey->getPrivateKey();

    ctx.mdctx = EVP_MD_CTX_new();
    if (ctx.mdctx == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to create MD_CTX");

    // initialize signing
    if (!EVP_DigestSignInit_ex(ctx.mdctx, nullptr, nullptr, nullptr, nullptr, key, nullptr))
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "EVP_DigestSignInit_ex failed");

    // perform preflight to determine the length of the signature
    size_t length;
    if (!EVP_DigestSign(ctx.mdctx, nullptr, &length, data.data(), data.size()))
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "EVP_DigestSign failed");

    std::vector<tu_uint8> bytes;
    bytes.resize(length);

    // generate the signature
    if (!EVP_DigestSign(ctx.mdctx, bytes.data(), &length, data.data(), data.size()))
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "EVP_DigestSign failed");

    Digest digest(std::move(bytes));
    return digest;
}

tempo_utils::Result<tempo_security::Digest>
tempo_security::DigestUtils::generate_signed_message_digest(
    std::span<const tu_uint8> data,
    const std::filesystem::path &pemPrivateKeyFile,
    DigestId digestId)
{
    std::shared_ptr<PrivateKey> privateKey;
    TU_ASSIGN_OR_RETURN (privateKey, PrivateKey::readFile(pemPrivateKeyFile));
    return generate_signed_message_digest(data, privateKey, digestId);
}

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct VerifySignedDigestCtx {
    EVP_MD_CTX *mdctx = nullptr;
    EVP_PKEY *key = nullptr;

    ~VerifySignedDigestCtx() {
        if (key)
            EVP_PKEY_free(key);
        if (mdctx)
            EVP_MD_CTX_free(mdctx);
    }
};

tempo_utils::Result<bool>
tempo_security::DigestUtils::verify_signed_message_digest(
    std::span<const tu_uint8> data,
    const Digest &digest,
    std::shared_ptr<X509Certificate> certificate)
{
    VerifySignedDigestCtx ctx;
    auto *crt = certificate->getCertificate();

    ctx.key = X509_get_pubkey(crt);
    if (ctx.key == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kParseError,
            "failed to get public key from certificate");

    ctx.mdctx = EVP_MD_CTX_new();
    if (ctx.mdctx == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to create MD_CTX");

    int ret;

    // initialize verification
    if (!EVP_DigestVerifyInit_ex(ctx.mdctx, nullptr, nullptr, nullptr, nullptr, ctx.key, nullptr))
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "EVP_DigestVerifyInit_ex failed");

    // verify the signature
    ret = EVP_DigestVerify(ctx.mdctx, digest.getData(), digest.getSize(), data.data(), data.size());
    if (ret < 0)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "EVP_DigestVerify failed");

    return ret == 1;
}

tempo_utils::Result<bool>
tempo_security::DigestUtils::verify_signed_message_digest(
    std::span<const tu_uint8> data,
    const Digest &digest,
    const std::filesystem::path &pemCertificateFile)
{
    std::shared_ptr<X509Certificate> certificate;
    TU_ASSIGN_OR_RETURN (certificate, X509Certificate::readFile(pemCertificateFile));
    return verify_signed_message_digest(data, digest, certificate);
}