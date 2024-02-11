#ifndef TEMPO_SECURITY_ECC_PRIVATE_KEY_GENERATOR_H
#define TEMPO_SECURITY_ECC_PRIVATE_KEY_GENERATOR_H

#include "abstract_private_key_generator.h"

namespace tempo_security {

    class ECCPrivateKeyGenerator : public AbstractPrivateKeyGenerator {

    public:
        ECCPrivateKeyGenerator(int ecCurveNid);

        KeyType getKeyType() const override;
        EVP_PKEY *generatePrivateKey() const override;

    private:
        int m_ecCurveNid;
    };
}

#endif // TEMPO_SECURITY_ECC_PRIVATE_KEY_GENERATOR_H