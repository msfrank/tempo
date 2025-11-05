
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

inline const EVP_MD *
get_digest(tempo_security::DigestId digestId)
{
    switch (digestId) {
        case tempo_security::DigestId::None:
            return nullptr;
        case tempo_security::DigestId::SHA256:
            return EVP_sha256();
        case tempo_security::DigestId::SHA384:
            return EVP_sha384();
        case tempo_security::DigestId::SHA512:
            return EVP_sha512();
        case tempo_security::DigestId::SHA3_256:
            return EVP_sha3_256();
        case tempo_security::DigestId::SHA3_384:
            return EVP_sha3_384();
        case tempo_security::DigestId::SHA3_512:
            return EVP_sha3_512();
        default:
            return nullptr;
    }
}

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct GenerateSignedDigestCtx {
    EVP_MD_CTX *mdctx = nullptr;
    EVP_PKEY *key = nullptr;

    ~GenerateSignedDigestCtx() {
        if (key)
            EVP_PKEY_free(key);
        if (mdctx)
            EVP_MD_CTX_free(mdctx);
    }
};

tempo_utils::Result<tempo_security::Digest>
tempo_security::generate_signed_message_digest(
    std::span<const tu_uint8> data,
    const std::filesystem::path &pemPrivateKeyFile,
    DigestId digestId)
{
    GenerateSignedDigestCtx ctx;

    // load the private key
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

    // verify the key type is supported
    auto type = EVP_PKEY_type(EVP_PKEY_get_id(ctx.key));
    switch (type) {
        case EVP_PKEY_EC:
        case EVP_PKEY_RSA:
        case EVP_PKEY_ED25519:
            break;
        default:
            return SecurityStatus::forCondition(SecurityCondition::kMissingPrivateKey,
                "unsupported private key {}", pemPrivateKeyFile.string());
    }

    ctx.mdctx = EVP_MD_CTX_new();
    if (ctx.mdctx == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to create MD_CTX");

    // initialize signing
    if (!EVP_DigestSignInit_ex(ctx.mdctx, nullptr, nullptr, nullptr, nullptr, ctx.key, nullptr))
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

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct VerifySignedDigestCtx {
    EVP_MD_CTX *mdctx = nullptr;
    X509 *crt = nullptr;
    EVP_PKEY *key = nullptr;

    ~VerifySignedDigestCtx() {
        if (key)
            EVP_PKEY_free(key);
        if (crt)
            X509_free(crt);
        if (mdctx)
            EVP_MD_CTX_free(mdctx);
    }
};

tempo_utils::Result<bool>
tempo_security::verify_signed_message_digest(
    std::span<const tu_uint8> data,
    const Digest &digest,
    const std::filesystem::path &pemCertificateFile,
    DigestId digestId)
{
    VerifySignedDigestCtx ctx;

    // load the public key
    auto *key_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(key_bio, pemCertificateFile.c_str())) {
        ctx.crt = PEM_read_bio_X509(key_bio, NULL, NULL, NULL);
        BIO_free_all(key_bio);
        if (ctx.crt == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid certificate {}", pemCertificateFile.string());
        ctx.key = X509_get_pubkey(ctx.crt);
        if (ctx.key == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "failed to get public key from certificate {}", pemCertificateFile.string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingPrivateKey,
            "missing certificate {}", pemCertificateFile.string());
    }

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