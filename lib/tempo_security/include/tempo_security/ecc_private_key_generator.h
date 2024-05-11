#ifndef TEMPO_SECURITY_ECC_PRIVATE_KEY_GENERATOR_H
#define TEMPO_SECURITY_ECC_PRIVATE_KEY_GENERATOR_H

#include "abstract_private_key_generator.h"

namespace tempo_security {

    enum class ECCurveId {
        Prime256v1,
    };

    class ECCPrivateKeyGenerator : public AbstractPrivateKeyGenerator {

    public:
        explicit ECCPrivateKeyGenerator(ECCurveId curveId);
        explicit ECCPrivateKeyGenerator(int ecCurveNid);

        KeyType getKeyType() const override;
        EVP_PKEY *generatePrivateKey() const override;
        bool isValidPrivateKey(const std::filesystem::path &pemPrivateKeyFile) const override;

    private:
        int m_ecCurveNid;
    };
}

#endif // TEMPO_SECURITY_ECC_PRIVATE_KEY_GENERATOR_H