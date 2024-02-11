#ifndef TEMPO_SECURITY_ABSTRACT_PRIVATE_KEY_GENERATOR_H
#define TEMPO_SECURITY_ABSTRACT_PRIVATE_KEY_GENERATOR_H

#include <openssl/evp.h>

#include "security_types.h"

namespace tempo_security {

    class AbstractPrivateKeyGenerator {

    public:
        virtual ~AbstractPrivateKeyGenerator() = default;

        /**
         *
         * @return
         */
        virtual KeyType getKeyType() const = 0;

        /**
         *
         * @return
         */
        virtual EVP_PKEY *generatePrivateKey() const = 0;
    };
}

#endif // TEMPO_SECURITY_ABSTRACT_PRIVATE_KEY_GENERATOR_H