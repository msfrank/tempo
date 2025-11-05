#ifndef TEMPO_SECURITY_SECURITY_TYPES_H
#define TEMPO_SECURITY_SECURITY_TYPES_H

namespace tempo_security {

    enum class KeyType {
        INVALID,
        RSA,
        ECC,
        ED25519
    };
    constexpr int kRSAKeyBits = 2048;
    constexpr int kRSAPublicExponent = 65537;

}

#endif // TEMPO_SECURITY_SECURITY_TYPES_H