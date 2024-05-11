
#include <tempo_utils/log_message.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>

#include <tempo_security/x509_store.h>

tempo_security::X509Store::X509Store()
    : m_store(nullptr)
{
}

tempo_security::X509Store::X509Store(X509_STORE *store)
    : m_store(store)
{
    TU_ASSERT (m_store != nullptr);
}

tempo_security::X509Store::~X509Store()
{
    X509_STORE_free(m_store);
}

tempo_utils::Status
tempo_security::X509Store::verifyCertificate(const std::filesystem::path &pemCertificateFile) const
{
    if (!exists(pemCertificateFile))
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "invalid certificate {}", pemCertificateFile.string());

    auto loadCertificateResult = X509Certificate::readFile(pemCertificateFile);
    if (loadCertificateResult.isStatus())
        return loadCertificateResult.getStatus();

    return verifyCertificate(loadCertificateResult.getResult());
}

tempo_utils::Status
tempo_security::X509Store::verifyCertificate(std::shared_ptr<X509Certificate> x509Certificate) const
{
    TU_ASSERT (x509Certificate != nullptr);

    if (!x509Certificate->isValid())
        return SecurityStatus::forCondition(SecurityCondition::kInvalidCertificate,
            "certificate is invalid");

    X509 *x509 = x509Certificate->m_x509;

    tempo_utils::Status status(tempo_utils::StatusCode::kInternal);

    auto *ctx = X509_STORE_CTX_new();
    if (X509_STORE_CTX_init(ctx, m_store, x509, nullptr)) {

        X509_STORE_CTX_set_purpose(ctx, X509_PURPOSE_ANY);

        if (!X509_verify_cert(ctx)) {
            auto error = X509_STORE_CTX_get_error(ctx);
            auto depth = X509_STORE_CTX_get_error_depth(ctx);
            auto *cause = X509_verify_cert_error_string(error);
            status = SecurityStatus::forCondition(SecurityCondition::kVerificationFailure,
                "cert verification failed at depth {}: {}", depth, cause);
        } else {
            status = SecurityStatus::ok();
        }
    }

    X509_STORE_CTX_free(ctx);

    return status;
}

std::vector<std::shared_ptr<tempo_security::X509Certificate>>
tempo_security::X509Store::listCertificates() const
{
    std::vector<std::shared_ptr<X509Certificate>> certificates;

    auto *storeCerts = X509_STORE_get1_all_certs(m_store);

    for (int i = 0; i < sk_X509_num(storeCerts); i++) {
        auto *x509 = sk_X509_value(storeCerts, i);
        auto cert = std::shared_ptr<X509Certificate>(new X509Certificate(x509));
        certificates.push_back(cert);
    }
    sk_X509_free(storeCerts);

    return certificates;
}

tempo_utils::Result<std::shared_ptr<tempo_security::X509Store>>
tempo_security::X509Store::loadLocations(
    const X509StoreOptions &options,
    const std::filesystem::path &caCertsDirectory,
    const std::filesystem::path &caCertsFile)
{
    auto *store = X509_STORE_new();

    // X509_STORE_set_depth(store, options.depth);
    // X509_STORE_set_purpose(store, options.purpose);
    // X509_STORE_set_trust(store, options.trust);
    // X509_STORE_set_flags(store, options.flags);
    // if (options.loadFromDefaultPaths)
    //     X509_STORE_set_default_paths(store);

    auto caCertsFileString = caCertsFile.empty()? nullptr : caCertsFile.c_str();
    auto caCertsDirectoryString = caCertsDirectory.empty()? nullptr : caCertsDirectory.c_str();

    if (!X509_STORE_load_locations(store, caCertsFileString, caCertsDirectoryString)) {
        auto errnum = ERR_get_error();
        const char *error = ERR_reason_error_string(errnum);
        X509_STORE_free(store);
        return SecurityStatus::forCondition(SecurityCondition::kInvalidTrustStore,
            "failed to load locations: {}", error);
    }

    return std::shared_ptr<X509Store>(new X509Store(store));
}

tempo_utils::Result<std::shared_ptr<tempo_security::X509Store>>
tempo_security::X509Store::loadTrustedCerts(
    const X509StoreOptions &options,
    const std::vector<std::filesystem::path> &trustedCerts)
{
    auto *store = X509_STORE_new();

    for (const auto &trustedCert : trustedCerts) {
        X509 *x509 = nullptr;
        auto *bio = BIO_new_file(trustedCert.c_str(), "rx");
        if (bio == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kMissingCertificate,
                "missing certificate {}", trustedCert.string());

        PEM_read_bio_X509(bio, &x509, nullptr, nullptr);
        BIO_free(bio);

        if (x509 == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kInvalidCertificate,
                "invalid certificate {}", trustedCert.string());

        X509_STORE_add_cert(store, x509);
    }

    return std::shared_ptr<X509Store>(new X509Store(store));
}
