#ifndef TEMPO_SECURITY_CERTIFICATE_KEY_PAIR_H
#define TEMPO_SECURITY_CERTIFICATE_KEY_PAIR_H

#include <filesystem>

#include "security_result.h"
#include "security_types.h"

namespace tempo_security {

    class CertificateKeyPair {

    public:
        CertificateKeyPair();
        CertificateKeyPair(
            KeyType keyType,
            const std::filesystem::path &pemPrivateKeyFile,
            const std::filesystem::path &pemCertificateFile);
        CertificateKeyPair(const CertificateKeyPair &other);

        bool isValid() const;

        KeyType getKeyType() const;
        std::filesystem::path getPemPrivateKeyFile() const;
        std::filesystem::path getPemCertificateFile() const;

        static tempo_utils::Result<CertificateKeyPair> load(
            const std::filesystem::path &pemPrivateKeyFile,
            const std::filesystem::path &pemCertificateFile);

    private:
        KeyType m_keyType;
        std::filesystem::path m_pemPrivateKeyFile;
        std::filesystem::path m_pemCertificateFile;
    };
}

#endif // TEMPO_SECURITY_CERTIFICATE_KEY_PAIR_H