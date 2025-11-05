#ifndef TEMPO_SECURITY_DIGEST_UTILS_H
#define TEMPO_SECURITY_DIGEST_UTILS_H

#include <tempo_utils/result.h>

namespace tempo_security {

    enum class DigestId {
        None,
        SHA256,
        SHA384,
        SHA512,
        SHA3_256,
        SHA3_384,
        SHA3_512,
    };

    class Digest {
    public:
        Digest() = default;
        Digest(std::vector<tu_uint8> &&data);
        Digest(const Digest &other);

        bool isValid() const;

        const tu_uint8 *getData() const;
        size_t getSize() const;
        std::span<const tu_uint8> getSpan() const;

    private:
        std::shared_ptr<std::vector<tu_uint8>> m_data;
    };

    tempo_utils::Result<Digest> generate_signed_message_digest(
        std::span<const tu_uint8> data,
        const std::filesystem::path &pemPrivateKeyFile,
        DigestId digestId);

    tempo_utils::Result<bool> verify_signed_message_digest(
        std::span<const tu_uint8> data,
        const Digest &digest,
        const std::filesystem::path &pemCertificateFile,
        DigestId digestId);

}

#endif // TEMPO_SECURITY_DIGEST_UTILS_H