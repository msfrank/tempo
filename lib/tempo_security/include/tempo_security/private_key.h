#ifndef TEMPO_SECURITY_PRIVATE_KEY_H
#define TEMPO_SECURITY_PRIVATE_KEY_H

#include <openssl/evp.h>

#include <tempo_utils/result.h>

#include "security_types.h"

namespace tempo_security {

    class PrivateKey {
    public:
        virtual ~PrivateKey();

        KeyType getKeyType() const;

        std::string toString() const;

        static tempo_utils::Result<std::shared_ptr<PrivateKey>> readFile(
            const std::filesystem::path &pemPrivateKeyFile);
        static tempo_utils::Result<std::shared_ptr<PrivateKey>> fromString(
            std::string_view pemPrivateKeyString);

    private:
        EVP_PKEY *m_pkey;

        explicit PrivateKey(EVP_PKEY *pkey);
        EVP_PKEY *getPrivateKey() const;

        friend class CertificateKeyPair;
        friend class CSRKeyPair;
        friend class DigestUtils;
    };
}

#endif // TEMPO_SECURITY_PRIVATE_KEY_H