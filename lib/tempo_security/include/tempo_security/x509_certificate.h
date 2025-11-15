#ifndef TEMPO_SECURITY_X509_CERTIFICATE_H
#define TEMPO_SECURITY_X509_CERTIFICATE_H

#include <memory>

#include <openssl/x509.h>

#include <tempo_utils/file_reader.h>
#include <tempo_utils/result.h>

namespace tempo_security {

    class X509Certificate {

    public:
        ~X509Certificate();

        long getVersion() const;
        long getSerialNumber() const;
        std::string getOrganization() const;
        std::string getOrganizationalUnit() const;
        std::string getCommonName() const;
        std::chrono::time_point<std::chrono::system_clock> getNotValidBefore() const;
        std::chrono::time_point<std::chrono::system_clock> getNotValidAfter() const;
        bool isCertificateAuthority() const;

        std::string toPem() const;
        std::string toString() const;

        bool operator==(const X509Certificate &other) const;
        bool operator!=(const X509Certificate &other) const;

        static tempo_utils::Result<std::shared_ptr<X509Certificate>> readFile(
            const std::filesystem::path &pemCertificateFile);
        static tempo_utils::Result<std::shared_ptr<X509Certificate>> fromString(
            std::string_view pemCertificateString);

    private:
        X509 *m_x509;

        explicit X509Certificate(X509 *x509);
        X509 *getCertificate() const;

        friend class CertificateKeyPair;
        friend class DigestUtils;
        friend class X509Store;
    };


}

#endif // TEMPO_SECURITY_X509_CERTIFICATE_H