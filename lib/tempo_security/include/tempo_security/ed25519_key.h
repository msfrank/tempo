#ifndef TEMPO_SECURITY_ED25519_KEY_H
#define TEMPO_SECURITY_ED25519_KEY_H

#include <memory>

#include <openssl/types.h>

#include <tempo_utils/file_reader.h>

namespace tempo_security {

    class Ed25519Key {

    public:
        Ed25519Key(const std::filesystem::path &pemPrivateKeyFile);
        ~Ed25519Key();

        // we disallow any copying or moving of the Ed25519Key
        Ed25519Key(const Ed25519Key &&other) = delete;
        Ed25519Key(const Ed25519Key &other) = delete;
        Ed25519Key& operator=(Ed25519Key &&other) noexcept = delete;
        Ed25519Key& operator=(const Ed25519Key &other) = delete;

        bool isValid() const;
        tempo_utils::Status getStatus() const;
        std::filesystem::path getPemPrivateKeyFile() const;
        std::string toString() const;

    private:
        std::filesystem::path m_pemPrivateKeyFile;
        tempo_utils::Status m_status;
        EVP_PKEY *m_key;
    };
}

#endif // TEMPO_SECURITY_ED25519_KEY_H