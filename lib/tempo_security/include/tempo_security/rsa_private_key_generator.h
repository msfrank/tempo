#ifndef TEMPO_SECURITY_RSA_PRIVATE_KEY_GENERATOR_H
#define TEMPO_SECURITY_RSA_PRIVATE_KEY_GENERATOR_H

#include "abstract_private_key_generator.h"

namespace tempo_security {

    class RSAPrivateKeyGenerator : public AbstractPrivateKeyGenerator {

    public:
        RSAPrivateKeyGenerator(int keyBits, int publicExponent);

        KeyType getKeyType() const override;
        EVP_PKEY *generatePrivateKey() const override;

    private:
        int m_keyBits;
        int m_publicExponent;
    };
}

#endif // TEMPO_SECURITY_RSA_PRIVATE_KEY_GENERATOR_H