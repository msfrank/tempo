#ifndef TEMPO_SECURITY_SECURITY_TYPES_H
#define TEMPO_SECURITY_SECURITY_TYPES_H

#include <openssl/types.h>

namespace tempo_security {

    constexpr int kRSAKeyBits = 2048;
    constexpr int kRSAPublicExponent = 65537;

    enum class KeyType {
        Invalid,
        Rsa,
        Ecdsa,
        Ed25519
    };

    enum class DigestId {
        None,
        SHA256,
        SHA384,
        SHA512,
        SHA3_256,
        SHA3_384,
        SHA3_512,
    };

    const EVP_MD *get_digest(DigestId digestId);

}

#endif // TEMPO_SECURITY_SECURITY_TYPES_H