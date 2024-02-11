#ifndef TEMPO_SECURITY_X509_STORE_H
#define TEMPO_SECURITY_X509_STORE_H

#include <filesystem>
#include <memory>
#include <vector>

#include <openssl/crypto.h>
#include <openssl/x509.h>

#include "security_result.h"
#include "x509_certificate.h"

namespace tempo_security {

    struct X509StoreOptions {
        int depth = 0;
        unsigned long flags = 0;
        int purpose = 0;
        int trust = 0;
        bool loadFromDefaultPaths = false;
    };

    class X509Store {

    public:
        ~X509Store();

        tempo_utils::Status verifyCertificate(std::shared_ptr<X509Certificate> x509Certificate) const;
        tempo_utils::Status verifyCertificate(const std::filesystem::path &pemCertificateFile) const;

        std::vector<std::shared_ptr<X509Certificate>> listCertificates() const;

        static tempo_utils::Result<std::shared_ptr<X509Store>> loadLocations(
            const X509StoreOptions &options,
            const std::filesystem::path &caCertsDirectory = {},
            const std::filesystem::path &caCertsFile = {});
        static tempo_utils::Result<std::shared_ptr<X509Store>> loadTrustedCerts(
            const X509StoreOptions &options,
            const std::vector<std::filesystem::path> &trustedCerts = {});

    private:
        X509_STORE *m_store;

        X509Store();
        explicit X509Store(X509_STORE *store);
    };
}

#endif // TEMPO_SECURITY_X509_STORE_H