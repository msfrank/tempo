#ifndef TEMPO_SECURITY_RSA_KEY_H
#define TEMPO_SECURITY_RSA_KEY_H

#include <memory>

#include <openssl/rsa.h>

#include <tempo_utils/file_reader.h>

namespace tempo_security {

    class RSAKey {

    public:
        RSAKey(const std::filesystem::path &pemPrivateKeyFile);
        ~RSAKey();

        // we disallow any copying or moving of the RSAKey
        RSAKey(const RSAKey &&other) = delete;
        RSAKey(const RSAKey &other) = delete;
        RSAKey& operator=(RSAKey &&other) noexcept = delete;
        RSAKey& operator=(const RSAKey &other) = delete;

        bool isValid() const;
        tempo_utils::Status getStatus() const;
        std::filesystem::path getPemPrivateKeyFile() const;
        std::string toString() const;

    private:
        std::filesystem::path m_pemPrivateKeyFile;
        tempo_utils::Status m_status;
        RSA *m_rsa;
    };
}

#endif // TEMPO_SECURITY_RSA_KEY_H