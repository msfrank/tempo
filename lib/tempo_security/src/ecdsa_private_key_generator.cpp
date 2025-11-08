
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/x509.h>

#include <tempo_security/ecdsa_private_key_generator.h>
#include <tempo_utils/log_message.h>

tempo_security::EcdsaPrivateKeyGenerator::EcdsaPrivateKeyGenerator(CurveId curveId)
{
    switch (curveId) {
        case CurveId::Prime256v1:
            m_curveNid = NID_X9_62_prime256v1;
            break;
        default:
            TU_UNREACHABLE();
    }
}

tempo_security::EcdsaPrivateKeyGenerator::EcdsaPrivateKeyGenerator(int ecCurveNid)
    : m_curveNid(ecCurveNid)
{
    TU_ASSERT (m_curveNid >= 0);
}

tempo_security::KeyType
tempo_security::EcdsaPrivateKeyGenerator::getKeyType() const
{
    return KeyType::Ecdsa;
}

EVP_PKEY *
tempo_security::EcdsaPrivateKeyGenerator::generatePrivateKey() const
{
    EC_KEY *ec = nullptr;
    EVP_PKEY *pkey = nullptr;

    ec = EC_KEY_new_by_curve_name(m_curveNid);
    pkey = EVP_PKEY_new();

    // Generate the private key
    if (!EC_KEY_generate_key(ec))
        goto err;
    if (!EVP_PKEY_assign_EC_KEY(pkey, ec))
        goto err;

    // note that pkey takes ownership of ec so we don't need to free it
    return pkey;

    err:
    if (ec)
        EC_KEY_free(ec);
    if (pkey)
        EVP_PKEY_free(pkey);
    return nullptr;
}