
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/x509.h>

#include <tempo_security/ecc_private_key_generator.h>
#include <tempo_utils/log_message.h>
#include "tempo_security/ecc_key.h"

tempo_security::ECCPrivateKeyGenerator::ECCPrivateKeyGenerator(ECCurveId curveId)
{
    switch (curveId) {
        case ECCurveId::Prime256v1:
            m_ecCurveNid = NID_X9_62_prime256v1;
            break;
        default:
            TU_UNREACHABLE();
    }
}

tempo_security::ECCPrivateKeyGenerator::ECCPrivateKeyGenerator(int ecCurveNid)
    : m_ecCurveNid(ecCurveNid)
{
    TU_ASSERT (m_ecCurveNid >= 0);
}

tempo_security::KeyType
tempo_security::ECCPrivateKeyGenerator::getKeyType() const
{
    return KeyType::ECC;
}

EVP_PKEY *
tempo_security::ECCPrivateKeyGenerator::generatePrivateKey() const
{
    EC_KEY *ec = nullptr;
    EVP_PKEY *pkey = nullptr;

    ec = EC_KEY_new_by_curve_name(m_ecCurveNid);
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

bool
tempo_security::ECCPrivateKeyGenerator::isValidPrivateKey(const std::filesystem::path &pemPrivateKeyFile) const
{
    ECCKey key(pemPrivateKeyFile);
    return key.isValid();
}
