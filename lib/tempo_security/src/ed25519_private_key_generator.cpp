
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/x509.h>

#include <tempo_security/ed25519_private_key_generator.h>
#include <tempo_utils/log_message.h>

tempo_security::Ed25519PrivateKeyGenerator::Ed25519PrivateKeyGenerator()
{
}

tempo_security::KeyType
tempo_security::Ed25519PrivateKeyGenerator::getKeyType() const
{
    return KeyType::Ed25519;
}

EVP_PKEY *
tempo_security::Ed25519PrivateKeyGenerator::generatePrivateKey() const
{
    EVP_PKEY_CTX *pctx = nullptr;
    EVP_PKEY *pkey = nullptr;

    pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
    if (pctx == nullptr)
        goto err;
    if (EVP_PKEY_keygen_init(pctx) <= 0)
        goto err;
    if (EVP_PKEY_keygen(pctx, &pkey) <= 0)
        goto err;

    EVP_PKEY_CTX_free(pctx);
    return pkey;

err:
    if (pkey)
        EVP_PKEY_free(pkey);
    if (pctx)
        EVP_PKEY_CTX_free(pctx);
    return nullptr;
}
