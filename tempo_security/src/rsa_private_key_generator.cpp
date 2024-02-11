
#include <openssl/evp.h>
#include <openssl/x509.h>

#include <tempo_security/rsa_private_key_generator.h>

tempo_security::RSAPrivateKeyGenerator::RSAPrivateKeyGenerator(int keyBits, int publicExponent)
    : m_keyBits(keyBits),
      m_publicExponent(publicExponent)
{
}

tempo_security::KeyType
tempo_security::RSAPrivateKeyGenerator::getKeyType() const
{
    return KeyType::RSA;
}

EVP_PKEY *
tempo_security::RSAPrivateKeyGenerator::generatePrivateKey() const
{
    RSA *rsa = nullptr;
    EVP_PKEY *pkey = nullptr;
    BIGNUM *e = nullptr;

    rsa = RSA_new();
    pkey = EVP_PKEY_new();
    e = BN_new();

    // Generate the private key
    BN_set_word(e, m_publicExponent);
    if (!RSA_generate_key_ex(rsa, m_keyBits, e, nullptr))
        goto err;
    if (!EVP_PKEY_assign_RSA(pkey, rsa))
        goto err;

    // note that pkey takes ownership of rsa so we don't need to free it
    BN_free(e);
    return pkey;

err:
    if (rsa)
        RSA_free(rsa);
    if (pkey)
        EVP_PKEY_free(pkey);
    if (e)
        BN_free(e);
    return nullptr;
}