#ifndef TEMPO_SECURITY_ECC_KEY_H
#define TEMPO_SECURITY_ECC_KEY_H

#include <memory>

#include <openssl/ec.h>

#include <tempo_utils/file_reader.h>

namespace tempo_security {

    class ECCKey {

    public:
        ECCKey(const std::filesystem::path &pemPrivateKeyFile);
        ~ECCKey();

        // we disallow any copying or moving of the ECCKey
        ECCKey(const ECCKey &&other) = delete;
        ECCKey(const ECCKey &other) = delete;
        ECCKey& operator=(ECCKey &&other) noexcept = delete;
        ECCKey& operator=(const ECCKey &other) = delete;

        bool isValid() const;
        tempo_utils::Status getStatus() const;
        std::filesystem::path getPemPrivateKeyFile() const;
        std::string toString() const;

    private:
        std::filesystem::path m_pemPrivateKeyFile;
        tempo_utils::Status m_status;
        EC_KEY *m_ecc;
    };
}

#endif // TEMPO_SECURITY_ECC_KEY_H
