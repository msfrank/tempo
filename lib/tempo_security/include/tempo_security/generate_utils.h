#ifndef TEMPO_SECURITY_GENERATE_UTILS_H
#define TEMPO_SECURITY_GENERATE_UTILS_H

#include <chrono>
#include <filesystem>
#include <string>

#include <tempo_utils/result.h>

#include "abstract_private_key_generator.h"
#include "certificate_key_pair.h"
#include "csr_key_pair.h"

namespace tempo_security {

    class GenerateUtils {
    public:

        static tempo_utils::Result<CertificateKeyPair> generate_ca_key_pair(
            const CertificateKeyPair &caKeyPair,
            const AbstractPrivateKeyGenerator &keygen,
            DigestId digestId,
            std::string_view organization,
            std::string_view organizationalUnit,
            std::string_view commonName,
            int serial,
            std::chrono::seconds validity,
            int pathlen,
            const std::filesystem::path &keyPairDestDirectory,
            std::string_view keyPairFilenameStem);

        static tempo_utils::Result<CertificateKeyPair> generate_self_signed_ca_key_pair(
            const AbstractPrivateKeyGenerator &keygen,
            DigestId digestId,
            std::string_view organization,
            std::string_view organizationalUnit,
            std::string_view commonName,
            int serial,
            std::chrono::seconds validity,
            int pathlen,
            const std::filesystem::path &keyPairDestDirectory,
            std::string_view keyPairFilenameStem);

        static tempo_utils::Result<CertificateKeyPair> generate_key_pair(
            const CertificateKeyPair &caKeyPair,
            const AbstractPrivateKeyGenerator &keygen,
            DigestId digestId,
            std::string_view organization,
            std::string_view organizationalUnit,
            std::string_view commonName,
            int serial,
            std::chrono::seconds validity,
            const std::filesystem::path &keyPairDestDirectory,
            std::string_view keyPairFilenameStem);

        static tempo_utils::Result<CertificateKeyPair> generate_self_signed_key_pair(
            const AbstractPrivateKeyGenerator &keygen,
            DigestId digestId,
            std::string_view organization,
            std::string_view organizationalUnit,
            std::string_view commonName,
            int serial,
            std::chrono::seconds validity,
            const std::filesystem::path &keyPairDestDirectory,
            std::string_view keyPairFilenameStem);

        static tempo_utils::Result<CSRKeyPair> generate_csr_key_pair(
            const AbstractPrivateKeyGenerator &keygen,
            DigestId digestId,
            std::string_view organization,
            std::string_view organizationalUnit,
            std::string_view commonName,
            const std::filesystem::path &keyPairDestDirectory,
            std::string_view keyPairFilenameStem);
    };
};

#endif // TEMPO_SECURITY_GENERATE_UTILS_H