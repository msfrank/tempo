#ifndef TEMPO_SECURITY_ECDSA_PRIVATE_KEY_GENERATOR_H
#define TEMPO_SECURITY_ECDSA_PRIVATE_KEY_GENERATOR_H

#include "abstract_private_key_generator.h"

namespace tempo_security {

    enum class CurveId {
        Prime256v1,
    };

    class EcdsaPrivateKeyGenerator : public AbstractPrivateKeyGenerator {

    public:
        explicit EcdsaPrivateKeyGenerator(CurveId curveId);
        explicit EcdsaPrivateKeyGenerator(int ecCurveNid);

        KeyType getKeyType() const override;
        EVP_PKEY *generatePrivateKey() const override;

    private:
        int m_curveNid;
    };
}

#endif // TEMPO_SECURITY_ECDSA_PRIVATE_KEY_GENERATOR_H