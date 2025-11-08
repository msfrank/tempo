#ifndef TEMPO_SECURITY_DIGEST_UTILS_H
#define TEMPO_SECURITY_DIGEST_UTILS_H

#include <tempo_utils/result.h>

#include "private_key.h"
#include "security_types.h"
#include "x509_certificate.h"

namespace tempo_security {

    class Digest {
    public:
        Digest() = default;
        explicit Digest(std::vector<tu_uint8> &&data);
        explicit Digest(std::span<const tu_uint8> data);
        Digest(const Digest &other);

        bool isValid() const;

        const tu_uint8 *getData() const;
        size_t getSize() const;
        std::span<const tu_uint8> getSpan() const;

    private:
        std::shared_ptr<std::vector<tu_uint8>> m_data;
    };

    class DigestUtils {
    public:

    static tempo_utils::Result<Digest> generate_signed_message_digest(
        std::span<const tu_uint8> data,
        std::shared_ptr<PrivateKey> privateKey,
        DigestId digestId);

    static tempo_utils::Result<Digest> generate_signed_message_digest(
        std::span<const tu_uint8> data,
        const std::filesystem::path &pemPrivateKeyFile,
        DigestId digestId);

    static tempo_utils::Result<bool> verify_signed_message_digest(
        std::span<const tu_uint8> data,
        const Digest &digest,
        std::shared_ptr<X509Certificate> certificate);

    static tempo_utils::Result<bool> verify_signed_message_digest(
        std::span<const tu_uint8> data,
        const Digest &digest,
        const std::filesystem::path &pemCertificateFile);
    };
}

#endif // TEMPO_SECURITY_DIGEST_UTILS_H