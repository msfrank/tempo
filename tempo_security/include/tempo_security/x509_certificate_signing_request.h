#ifndef TEMPO_SECURITY_X509_CERTIFICATE_SIGNING_REQUEST_H
#define TEMPO_SECURITY_X509_CERTIFICATE_SIGNING_REQUEST_H

#include <filesystem>

#include <openssl/x509.h>

#include <tempo_utils/result.h>

#include "certificate_key_pair.h"
#include "x509_certificate.h"

namespace tempo_security {

    class X509CertificateSigningRequest {

    public:
        ~X509CertificateSigningRequest();

        bool isValid() const;
        std::filesystem::path getPemRequestFile() const;
        std::string getOrganization() const;
        std::string getOrganizationalUnit() const;
        std::string getCommonName() const;
        std::string toString() const;

        static tempo_utils::Result<std::shared_ptr<X509CertificateSigningRequest>> readFile(
            const std::filesystem::path &pemCertificateFile);

    private:
        std::filesystem::path m_pemRequestFile;
        X509_REQ *m_req;

        X509CertificateSigningRequest();
        X509CertificateSigningRequest(const std::filesystem::path &pemRequestFile, X509_REQ *req);
    };

    struct CSRValidationParams {

    };

    tempo_utils::Result<std::string> generate_certificate_from_csr(
        std::string_view pemRequestBytes,
        const CertificateKeyPair &caKeyPair,
        int serial,
        std::chrono::seconds validity);

    tempo_utils::Result<std::filesystem::path> generate_certificate_from_csr(
        const std::filesystem::path &pemRequestFile,
        const CertificateKeyPair &caKeyPair,
        int serial,
        std::chrono::seconds validity,
        const std::filesystem::path &certificateDestDirectory,
        std::string_view certificateFilenameStem);
}

#endif // TEMPO_SECURITY_X509_CERTIFICATE_SIGNING_REQUEST_H