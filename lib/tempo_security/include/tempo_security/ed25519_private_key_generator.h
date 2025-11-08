#ifndef TEMPO_SECURITY_ED25519_PRIVATE_KEY_GENERATOR_H
#define TEMPO_SECURITY_ED25519_PRIVATE_KEY_GENERATOR_H

#include "abstract_private_key_generator.h"

namespace tempo_security {

    class Ed25519PrivateKeyGenerator : public AbstractPrivateKeyGenerator {

    public:
        explicit Ed25519PrivateKeyGenerator();

        KeyType getKeyType() const override;
        EVP_PKEY *generatePrivateKey() const override;
    };
}

#endif //T EMPO_SECURITY_ED25519_PRIVATE_KEY_GENERATOR_H