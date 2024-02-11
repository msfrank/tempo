
#include <openssl/conf.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

#include <tempo_security/security_result.h>
#include <tempo_security/x509_certificate_signing_request.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/posix_result.h>

tempo_security::X509CertificateSigningRequest::X509CertificateSigningRequest()
    : m_req(nullptr)
{
}

tempo_security::X509CertificateSigningRequest::X509CertificateSigningRequest(
    const std::filesystem::path &pemRequestFile,
    X509_REQ *req)
    : m_pemRequestFile(pemRequestFile),
      m_req(req)
{
    TU_ASSERT (m_req != nullptr);
}

tempo_security::X509CertificateSigningRequest::~X509CertificateSigningRequest()
{
    X509_REQ_free(m_req);
}

bool
tempo_security::X509CertificateSigningRequest::isValid() const
{
    return m_req != nullptr;
}

std::filesystem::path
tempo_security::X509CertificateSigningRequest::getPemRequestFile() const
{
    return m_pemRequestFile;
}

std::string
tempo_security::X509CertificateSigningRequest::getOrganization() const
{
    if (!isValid())
        return {};
    auto *dn = X509_REQ_get_subject_name(m_req);

    std::string organization;
    int lastpos = -1;
    bool found = false;

    for (;;) {
        lastpos = X509_NAME_get_index_by_NID(dn, NID_organizationName, lastpos);
        if (lastpos == -1)
            break;
        if (found == true)
            return {};
        auto *entry = X509_NAME_get_entry(dn, lastpos);
        auto *data = X509_NAME_ENTRY_get_data(entry);
        char *utf8 = nullptr;
        auto len = ASN1_STRING_to_UTF8((unsigned char **) &utf8, data);
        if (len < 0)
            return {};
        organization = std::string(utf8, len);
        OPENSSL_free(utf8);
        found = true;
    }

    return organization;
}

std::string
tempo_security::X509CertificateSigningRequest::getOrganizationalUnit() const
{
    if (!isValid())
        return {};
    auto *dn = X509_REQ_get_subject_name(m_req);

    std::string organizationalUnit;
    int lastpos = -1;
    bool found = false;

    for (;;) {
        lastpos = X509_NAME_get_index_by_NID(dn, NID_organizationalUnitName, lastpos);
        if (lastpos == -1)
            break;
        if (found == true)
            return {};
        auto *entry = X509_NAME_get_entry(dn, lastpos);
        auto *data = X509_NAME_ENTRY_get_data(entry);
        char *utf8 = nullptr;
        auto len = ASN1_STRING_to_UTF8((unsigned char **) &utf8, data);
        if (len < 0)
            return {};
        organizationalUnit = std::string(utf8, len);
        OPENSSL_free(utf8);
        found = true;
    }

    return organizationalUnit;
}

std::string
tempo_security::X509CertificateSigningRequest::getCommonName() const
{
    if (!isValid())
        return {};
    auto *dn = X509_REQ_get_subject_name(m_req);

    std::string commonName;
    int lastpos = -1;
    bool found = false;

    for (;;) {
        lastpos = X509_NAME_get_index_by_NID(dn, NID_commonName, lastpos);
        if (lastpos == -1)
            break;
        if (found == true)
            return {};
        auto *entry = X509_NAME_get_entry(dn, lastpos);
        auto *data = X509_NAME_ENTRY_get_data(entry);
        char *utf8 = nullptr;
        auto len = ASN1_STRING_to_UTF8((unsigned char **) &utf8, data);
        if (len < 0)
            return {};
        commonName = std::string(utf8, len);
        OPENSSL_free(utf8);
        found = true;
    }

    return commonName;
}

std::string
tempo_security::X509CertificateSigningRequest::toString() const
{
    auto *bio = BIO_new(BIO_s_mem());
    X509_REQ_print(bio, m_req);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string s(data, len);
    BIO_free(bio);
    return s;
}

tempo_utils::Result<std::shared_ptr<tempo_security::X509CertificateSigningRequest>>
tempo_security::X509CertificateSigningRequest::readFile(
    const std::filesystem::path &pemRequestFile)
{
    X509_REQ *req = nullptr;
    auto *bio = BIO_new_file(pemRequestFile.c_str(), "rx");
    if (bio == nullptr)
        goto err;
    if (!PEM_read_bio_X509_REQ(bio, &req, nullptr, nullptr))
        goto err;
    BIO_free(bio);
    return std::shared_ptr<X509CertificateSigningRequest>(
        new X509CertificateSigningRequest(pemRequestFile, req));

    err:
    if (bio != nullptr)
        BIO_free(bio);
    switch (errno) {
        case ENOENT:
            return tempo_utils::FileStatus::forCondition(
                tempo_utils::FileCondition::kFileNotFound,
                "certificate file {} not found", pemRequestFile.string());
        case EACCES:
            return tempo_utils::FileStatus::forCondition(
                tempo_utils::FileCondition::kAccessDenied,
                "access denied for {}", pemRequestFile.string());
        default:
            return tempo_utils::PosixStatus::fromError(errno);
    }
}

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct SignCertCtx {
    EVP_PKEY *ca_key = nullptr;
    X509 *ca_crt = nullptr;
    X509_REQ *req = nullptr;
    X509 *crt = nullptr;

    ~SignCertCtx() {
        if (ca_key)
            EVP_PKEY_free(ca_key);
        if (ca_crt)
            X509_free(ca_crt);
        if (req)
            X509_REQ_free(req);
        if (crt)
            X509_free(crt);
    }
};

static tempo_utils::Status
generate_certificate_from_X509_REQ(
    SignCertCtx &ctx,
    const tempo_security::CertificateKeyPair &caKeyPair,
    int serial,
    std::chrono::seconds validity)
{
    TU_ASSERT (ctx.req != nullptr);

    // load the CA cert
    auto *cacrt_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(cacrt_bio, caKeyPair.getPemCertificateFile().c_str())) {
        ctx.ca_crt = PEM_read_bio_X509(cacrt_bio, NULL, NULL, NULL);
        BIO_free_all(cacrt_bio);
        if (ctx.ca_crt == nullptr)
            return tempo_security::SecurityStatus::forCondition(
                tempo_security::SecurityCondition::kParseError,
                "failed to read certificate file {}", caKeyPair.getPemCertificateFile().string());
    } else {
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kMissingCertificate,
            "missing certificate file {}", caKeyPair.getPemCertificateFile().string());
    }

    // load the CA private key
    auto *cakey_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(cakey_bio, caKeyPair.getPemPrivateKeyFile().c_str())) {
        ctx.ca_key = PEM_read_bio_PrivateKey(cakey_bio, NULL, NULL, NULL);
        BIO_free_all(cakey_bio);
        if (ctx.ca_key == nullptr)
            return tempo_security::SecurityStatus::forCondition(
                tempo_security::SecurityCondition::kParseError,
                "failed to read private key file {}", caKeyPair.getPemPrivateKeyFile().string());
    } else {
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kMissingPrivateKey,
            "missing private key file {}", caKeyPair.getPemPrivateKeyFile().string());
    }

    ctx.crt = X509_new();
    TU_ASSERT (ctx.crt != nullptr);

    // set version to X509v3
    X509_set_version(ctx.crt, 2);

    // set serial number
    ASN1_INTEGER_set(X509_get_serialNumber(ctx.crt), serial);

    // set issuer to CA's subject
    X509_set_issuer_name(ctx.crt, X509_get_subject_name(ctx.ca_crt));

    // set validity of certificate
    X509_gmtime_adj(X509_get_notBefore(ctx.crt), 0);
    X509_gmtime_adj(X509_get_notAfter(ctx.crt), static_cast<long>(validity.count()));

    // use the subject name from the csr without performing any validation
    X509_set_subject_name(ctx.crt, X509_REQ_get_subject_name(ctx.req));

    // use the public key from the csr
    EVP_PKEY *req_pubkey = X509_REQ_get_pubkey(ctx.req);
    X509_set_pubkey(ctx.crt, req_pubkey);
    EVP_PKEY_free(req_pubkey);

    // sign the certificate with the CA private key
    if (X509_sign(ctx.crt, ctx.ca_key, EVP_sha256()) == 0)
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kSigningFailure, "failed to sign certificate");

    // take ownership of crt from the ctx, then return the crt
    return tempo_security::SecurityStatus::ok();
}

tempo_utils::Result<std::string>
tempo_security::generate_certificate_from_csr(
    std::string_view pemRequestBytes,
    const CertificateKeyPair &caKeyPair,
    int serial,
    std::chrono::seconds validity)
{
    SignCertCtx ctx;

    // load the CSR bytes
    auto *csr_bio = BIO_new_mem_buf(pemRequestBytes.data(), pemRequestBytes.size());
    if (csr_bio != nullptr) {
        ctx.req = PEM_read_bio_X509_REQ(csr_bio, NULL, NULL, NULL);
        BIO_free_all(csr_bio);
        if (ctx.req == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid certificate signing request");
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingCertificateSigningRequest,
            "failed to read certificate signing request");
    }

    auto status = generate_certificate_from_X509_REQ(ctx, caKeyPair, serial, validity);
    TU_RETURN_IF_NOT_OK (status);

    // write the cert bytes
    auto *crt_bio = BIO_new(BIO_s_mem());
    TU_ASSERT (crt_bio != nullptr);
    if (!PEM_write_bio_X509(crt_bio, ctx.crt))
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kSecurityInvariant);
    char *data;
    auto len = BIO_get_mem_data(crt_bio, &data);
    std::string pemCertificateBytes(data, len);
    BIO_free_all(crt_bio);

    return pemCertificateBytes;
}

tempo_utils::Result<std::filesystem::path>
tempo_security::generate_certificate_from_csr(
    const std::filesystem::path &pemRequestFile,
    const CertificateKeyPair &caKeyPair,
    int serial,
    std::chrono::seconds validity,
    const std::filesystem::path &certificateDestDirectory,
    std::string_view certificateFilenameStem)
{
    SignCertCtx ctx;

    // load the CSR from the file
    auto *csr_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(csr_bio, pemRequestFile.c_str())) {
        ctx.req = PEM_read_bio_X509_REQ(csr_bio, NULL, NULL, NULL);
        BIO_free_all(csr_bio);
        if (ctx.req == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "failed to read certificate signing request file {}", pemRequestFile.string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingCertificateSigningRequest,
            "missing certificate signing request file {}", pemRequestFile.string());
    }

    // generate the certificate from the csr
    auto status = generate_certificate_from_X509_REQ(ctx, caKeyPair, serial, validity);
    TU_RETURN_IF_NOT_OK (status);

    std::filesystem::path pemCertificateFile = certificateDestDirectory / certificateFilenameStem;
    pemCertificateFile.replace_extension("crt");

    // write the cert
    auto *crt_bio = BIO_new_file(pemCertificateFile.c_str(), "wx");
    if (crt_bio == nullptr)
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kSigningFailure,
            "failed to write signed certificate to {}", pemCertificateFile.string());
    auto ret = PEM_write_bio_X509(crt_bio, ctx.crt);
    BIO_free_all(crt_bio);
    if (ret == 0)
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kSigningFailure,
            "failed to write signed certificate to {}", pemCertificateFile.string());

    return pemCertificateFile;
}