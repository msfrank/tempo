#ifndef TEMPO_SECURITY_CSR_KEY_PAIR_H
#define TEMPO_SECURITY_CSR_KEY_PAIR_H

#include <filesystem>

#include "abstract_private_key_generator.h"
#include "security_result.h"
#include "security_types.h"

namespace tempo_security {

    class CSRKeyPair {

    public:
        CSRKeyPair();
        CSRKeyPair(
            KeyType keyType,
            const std::filesystem::path &pemPrivateKeyFile,
            const std::filesystem::path &pemRequestFile);
        CSRKeyPair(const CSRKeyPair &other);

        bool isValid() const;

        KeyType getKeyType() const;
        std::filesystem::path getPemPrivateKeyFile() const;
        std::filesystem::path getPemRequestFile() const;

        static tempo_utils::Result<CSRKeyPair> load(
            const std::filesystem::path &pemPrivateKeyFile,
            const std::filesystem::path &pemRequestFile);

    private:
        KeyType m_keyType;
        std::filesystem::path m_pemPrivateKeyFile;
        std::filesystem::path m_pemRequestFile;
    };
}

#endif // TEMPO_SECURITY_CSR_KEY_PAIR_H