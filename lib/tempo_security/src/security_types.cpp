
#include <openssl/evp.h>

#include <tempo_security/security_types.h>

const EVP_MD *
tempo_security::get_digest(tempo_security::DigestId digestId)
{
    switch (digestId) {
    case tempo_security::DigestId::None:
        return nullptr;
    case tempo_security::DigestId::SHA256:
        return EVP_sha256();
    case tempo_security::DigestId::SHA384:
        return EVP_sha384();
    case tempo_security::DigestId::SHA512:
        return EVP_sha512();
    case tempo_security::DigestId::SHA3_256:
        return EVP_sha3_256();
    case tempo_security::DigestId::SHA3_384:
        return EVP_sha3_384();
    case tempo_security::DigestId::SHA3_512:
        return EVP_sha3_512();
    default:
        return nullptr;
    }
}
