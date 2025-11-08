
#include <absl/strings/str_cat.h>
#include <openssl/bioerr.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <tempo_security/generate_utils.h>
#include <tempo_security/security_result.h>
#include <tempo_utils/file_writer.h>
#include <tempo_utils/log_message.h>

static tempo_utils::Status
write_x509_csr(X509_REQ *req, const std::filesystem::path &pemRequestFile)
{
    TU_ASSERT (req != nullptr);

    auto *bio = BIO_new(BIO_s_mem());
    TU_ASSERT (bio != nullptr);
    if (!PEM_write_bio_X509_REQ(bio, req))
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kSecurityInvariant);

    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string_view bytes(data, len);

    tempo_utils::FileWriter fileWriter(pemRequestFile, bytes,
        tempo_utils::FileWriterMode::CREATE_ONLY, static_cast<std::filesystem::perms>(0644));
    BIO_free(bio);

    return fileWriter.getStatus();
}

static tempo_utils::Status
write_x509_certificate(X509 *cert, const std::filesystem::path &pemCertificateFile)
{
    TU_ASSERT (cert != nullptr);

    auto *bio = BIO_new(BIO_s_mem());
    TU_ASSERT (bio != nullptr);
    if (!PEM_write_bio_X509(bio, cert))
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kSecurityInvariant);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string_view bytes(data, len);

    tempo_utils::FileWriter fileWriter(pemCertificateFile, bytes,
        tempo_utils::FileWriterMode::CREATE_ONLY, static_cast<std::filesystem::perms>(0644));
    BIO_free(bio);

    return fileWriter.getStatus();
}

static tempo_utils::Status
write_private_key(EVP_PKEY *key, const std::filesystem::path &pemPrivateKeyFile)
{
    auto *bio = BIO_new(BIO_s_secmem());
    TU_ASSERT (bio != nullptr);
    if (!PEM_write_bio_PrivateKey(bio, key, nullptr, nullptr, 0, nullptr, nullptr))
        return tempo_security::SecurityStatus::forCondition(
            tempo_security::SecurityCondition::kSecurityInvariant);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string_view bytes(data, len);

    tempo_utils::FileWriter fileWriter(pemPrivateKeyFile, bytes,
        tempo_utils::FileWriterMode::CREATE_ONLY, static_cast<std::filesystem::perms>(0600));
    BIO_free(bio);

    return fileWriter.getStatus();
}

static
int add_x509_ext(X509 *cacrt, X509 *crt, int nid, const char *value)
{
    X509_EXTENSION *ex;
    X509V3_CTX ctx;

    X509V3_set_ctx_nodb(&ctx); // No configuration database
    X509V3_set_ctx(&ctx, cacrt, crt, nullptr, nullptr, 0);

    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
    if (!ex)
        return 0;

    X509_add_ext(crt, ex, -1);
    X509_EXTENSION_free(ex);
    return 1;
}

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct GenerateCAKeyPairCtx {
    EVP_PKEY *ca_key = nullptr;
    X509 *ca_crt = nullptr;
    EVP_PKEY *key = nullptr;
    X509 *crt = nullptr;

    ~GenerateCAKeyPairCtx() {
        if (ca_key)
            EVP_PKEY_free(ca_key);
        if (ca_crt)
            X509_free(ca_crt);
        if (key)
            EVP_PKEY_free(key);
        if (crt)
            X509_free(crt);
    }
};

tempo_utils::Result<tempo_security::CertificateKeyPair>
tempo_security::GenerateUtils::generate_ca_key_pair(
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
    std::string_view keyPairFilenameStem)
{
    GenerateCAKeyPairCtx ctx;

    if (caKeyPair.getKeyType() != keygen.getKeyType())
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "CA keypair key type must match the private key generator");

    // load the CA cert
    auto *cacrt_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(cacrt_bio, caKeyPair.getPemCertificateFile().c_str())) {
        ctx.ca_crt = PEM_read_bio_X509(cacrt_bio, NULL, NULL, NULL);
        BIO_free_all(cacrt_bio);
        if (ctx.ca_crt == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid CA certificate {}", caKeyPair.getPemCertificateFile().string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingCertificate,
            "missing CA certificate {}", caKeyPair.getPemCertificateFile().string());
    }

    // load the CA private key
    auto *cakey_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(cakey_bio, caKeyPair.getPemPrivateKeyFile().c_str())) {
        ctx.ca_key = PEM_read_bio_PrivateKey(cakey_bio, NULL, NULL, NULL);
        BIO_free_all(cakey_bio);
        if (ctx.ca_key == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid CA private key {}", caKeyPair.getPemPrivateKeyFile().string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingPrivateKey,
            "missing CA private key {}", caKeyPair.getPemPrivateKeyFile().string());
    }

    // generate the private key
    ctx.key = keygen.generatePrivateKey();
    if (ctx.key == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to generate private key");

    // allocate the cert
    ctx.crt = X509_new();
    TU_ASSERT (ctx.crt != nullptr);

    // set version to X509v3
    X509_set_version(ctx.crt, 2);

    // set serial number
    ASN1_INTEGER_set(X509_get_serialNumber(ctx.crt), serial);

    // set validity of certificate
    X509_gmtime_adj(X509_get_notBefore(ctx.crt), -30);
    X509_gmtime_adj(X509_get_notAfter(ctx.crt), static_cast<long>(validity.count()));

    // Set the DN of the request
    auto *dn = X509_get_subject_name(ctx.crt);
    //X509_NAME_add_entry_by_txt(dn, "C", MBSTRING_ASC, (const unsigned char*) "", -1, -1, 0);
    //X509_NAME_add_entry_by_txt(dn, "ST", MBSTRING_ASC, (const unsigned char*) "", -1, -1, 0);
    //X509_NAME_add_entry_by_txt(dn, "L", MBSTRING_ASC, (const unsigned char*) "", -1, -1, 0);
    X509_NAME_add_entry_by_txt(dn, "O", MBSTRING_ASC,
        (const unsigned char*) organization.data(), organization.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "OU", MBSTRING_ASC,
        (const unsigned char*) organizationalUnit.data(), organizationalUnit.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "CN", MBSTRING_ASC,
        (const unsigned char*) commonName.data(), commonName.length(), -1, 0);

    // set v3 extensions for a CA cert
    if (pathlen >= 0) {
        auto value = absl::StrCat("critical,CA:TRUE,pathlen:", pathlen);
        add_x509_ext(nullptr, ctx.crt, NID_basic_constraints, value.c_str());
    } else {
        add_x509_ext(nullptr, ctx.crt, NID_basic_constraints, "critical,CA:TRUE");
    }
    add_x509_ext(nullptr, ctx.crt, NID_key_usage, "critical,keyCertSign,cRLSign");
    add_x509_ext(nullptr, ctx.crt, NID_subject_key_identifier, "hash");

    // this is a self signed cert so set issuer to the same value as subject
    X509_set_issuer_name(ctx.crt, X509_get_subject_name(ctx.ca_crt));

    // set the public key
    X509_set_pubkey(ctx.crt, ctx.key);

    auto *md = get_digest(digestId);

    // sign the certificate with the CA private key
    if (X509_sign(ctx.crt, ctx.ca_key, md) == 0)
        return SecurityStatus::forCondition(SecurityCondition::kSigningFailure,
            "failed to sign certificate");

    std::filesystem::path pemCertificateFile = keyPairDestDirectory / keyPairFilenameStem;
    pemCertificateFile.replace_extension("crt");
    std::filesystem::path pemPrivateKeyFile = keyPairDestDirectory / keyPairFilenameStem;
    pemPrivateKeyFile.replace_extension("key");

    // write the cert
    TU_RETURN_IF_NOT_OK (write_x509_certificate(ctx.crt, pemCertificateFile));

    // write the private key
    TU_RETURN_IF_NOT_OK (write_private_key(ctx.key, pemPrivateKeyFile));

    // generation succeeded
    return CertificateKeyPair(keygen.getKeyType(), pemPrivateKeyFile, pemCertificateFile);
}

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct GenerateSelfSignedCAKeyPairCtx {
    EVP_PKEY *key = nullptr;
    X509 *crt = nullptr;

    ~GenerateSelfSignedCAKeyPairCtx() {
        if (key)
            EVP_PKEY_free(key);
        if (crt)
            X509_free(crt);
    }
};

tempo_utils::Result<tempo_security::CertificateKeyPair>
tempo_security::GenerateUtils::generate_self_signed_ca_key_pair(
    const AbstractPrivateKeyGenerator &keygen,
    DigestId digestId,
    std::string_view organization,
    std::string_view organizationalUnit,
    std::string_view commonName,
    int serial,
    std::chrono::seconds validity,
    int pathlen,
    const std::filesystem::path &keyPairDestDirectory,
    std::string_view keyPairFilenameStem)
{
    GenerateSelfSignedCAKeyPairCtx ctx;

    // generate the private key
    ctx.key = keygen.generatePrivateKey();
    if (ctx.key == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to generate private key");

    // allocate the cert
    ctx.crt = X509_new();
    TU_ASSERT (ctx.crt != nullptr);

    // set version to X509v3
    X509_set_version(ctx.crt, 2);

    // set serial number
    ASN1_INTEGER_set(X509_get_serialNumber(ctx.crt), serial);

    // set validity of certificate
    X509_gmtime_adj(X509_get_notBefore(ctx.crt), -30);
    X509_gmtime_adj(X509_get_notAfter(ctx.crt), static_cast<long>(validity.count()));

    // Set the DN of the request
    auto *dn = X509_get_subject_name(ctx.crt);
    //X509_NAME_add_entry_by_txt(dn, "C", MBSTRING_ASC, (const unsigned char*) "", -1, -1, 0);
    //X509_NAME_add_entry_by_txt(dn, "ST", MBSTRING_ASC, (const unsigned char*) "", -1, -1, 0);
    //X509_NAME_add_entry_by_txt(dn, "L", MBSTRING_ASC, (const unsigned char*) "", -1, -1, 0);
    X509_NAME_add_entry_by_txt(dn, "O", MBSTRING_ASC,
        (const unsigned char*) organization.data(), organization.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "OU", MBSTRING_ASC,
        (const unsigned char*) organizationalUnit.data(), organizationalUnit.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "CN", MBSTRING_ASC,
        (const unsigned char*) commonName.data(), commonName.length(), -1, 0);

    // set v3 extensions for a CA cert
    if (pathlen >= 0) {
        auto value = absl::StrCat("critical,CA:TRUE,pathlen:", pathlen);
        add_x509_ext(nullptr, ctx.crt, NID_basic_constraints, value.c_str());
    } else {
        add_x509_ext(nullptr, ctx.crt, NID_basic_constraints, "critical,CA:TRUE");
    }
    add_x509_ext(nullptr, ctx.crt, NID_key_usage, "critical,digitalSignature,keyCertSign,cRLSign");
    add_x509_ext(nullptr, ctx.crt, NID_ext_key_usage, "critical,serverAuth,clientAuth,codeSigning");
    add_x509_ext(nullptr, ctx.crt, NID_subject_key_identifier, "hash");

    // this is a self-signed cert so set issuer to the same value as subject
    X509_set_issuer_name(ctx.crt, dn);

    // set the public key
    X509_set_pubkey(ctx.crt, ctx.key);

    auto *md = get_digest(digestId);

    // sign the certificate with the CA private key
    if (X509_sign(ctx.crt, ctx.key, md) == 0)
        return SecurityStatus::forCondition(SecurityCondition::kSigningFailure,
            "failed to sign certificate");

    std::filesystem::path pemCertificateFile = keyPairDestDirectory / keyPairFilenameStem;
    pemCertificateFile.replace_extension("crt");
    std::filesystem::path pemPrivateKeyFile = keyPairDestDirectory / keyPairFilenameStem;
    pemPrivateKeyFile.replace_extension("key");

    // write the cert
    TU_RETURN_IF_NOT_OK (write_x509_certificate(ctx.crt, pemCertificateFile));

    // write the private key
    TU_RETURN_IF_NOT_OK (write_private_key(ctx.key, pemPrivateKeyFile));

    // generation succeeded
    return CertificateKeyPair(keygen.getKeyType(), pemPrivateKeyFile, pemCertificateFile);
}

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct GenerateKeyPairCtx {
    EVP_PKEY *ca_key = nullptr;
    X509 *ca_crt = nullptr;
    X509_REQ *req = nullptr;
    EVP_PKEY *key = nullptr;
    X509 *crt = nullptr;

    ~GenerateKeyPairCtx() {
        if (ca_key)
            EVP_PKEY_free(ca_key);
        if (ca_crt)
            X509_free(ca_crt);
        if (req)
            X509_REQ_free(req);
        if (key)
            EVP_PKEY_free(key);
        if (crt)
            X509_free(crt);
    }
};

tempo_utils::Result<tempo_security::CertificateKeyPair>
tempo_security::GenerateUtils::generate_key_pair(
    const CertificateKeyPair &caKeyPair,
    const AbstractPrivateKeyGenerator &keygen,
    DigestId digestId,
    std::string_view organization,
    std::string_view organizationalUnit,
    std::string_view commonName,
    int serial,
    std::chrono::seconds validity,
    const std::filesystem::path &keyPairDestDirectory,
    std::string_view keyPairFilenameStem)
{
    if (caKeyPair.getKeyType() != keygen.getKeyType())
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "CA keypair key type must match the private key generator");

    GenerateKeyPairCtx ctx;

    // load the CA cert
    auto *cacrt_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(cacrt_bio, caKeyPair.getPemCertificateFile().c_str())) {
        ctx.ca_crt = PEM_read_bio_X509(cacrt_bio, NULL, NULL, NULL);
        BIO_free_all(cacrt_bio);
        if (ctx.ca_crt == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid CA certificate {}", caKeyPair.getPemCertificateFile().string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingCertificate,
            "missing CA certificate {}", caKeyPair.getPemCertificateFile().string());
    }

    // load the CA private key
    auto *cakey_bio = BIO_new(BIO_s_file());
    if (BIO_read_filename(cakey_bio, caKeyPair.getPemPrivateKeyFile().c_str())) {
        ctx.ca_key = PEM_read_bio_PrivateKey(cakey_bio, NULL, NULL, NULL);
        BIO_free_all(cakey_bio);
        if (ctx.ca_key == nullptr)
            return SecurityStatus::forCondition(SecurityCondition::kParseError,
                "invalid CA private key {}", caKeyPair.getPemPrivateKeyFile().string());
    } else {
        return SecurityStatus::forCondition(SecurityCondition::kMissingPrivateKey,
            "missing CA private key {}", caKeyPair.getPemPrivateKeyFile().string());
    }

    // allocate the csr
    ctx.req = X509_REQ_new();
    TU_ASSERT (ctx.req != nullptr);

    // generate the private key
    ctx.key = keygen.generatePrivateKey();
    if (ctx.key == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to generate private key");

    X509_REQ_set_pubkey(ctx.req, ctx.key);

    // Set the DN of the request
    auto *dn = X509_REQ_get_subject_name(ctx.req);
    X509_NAME_add_entry_by_txt(dn, "O", MBSTRING_ASC,
        (const unsigned char*) organization.data(), organization.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "OU", MBSTRING_ASC,
        (const unsigned char*) organizationalUnit.data(), organizationalUnit.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "CN", MBSTRING_ASC,
        (const unsigned char*) commonName.data(), commonName.length(), -1, 0);

    auto *md = get_digest(digestId);

    // Self-sign the request to prove that we possess the key
    if (!X509_REQ_sign(ctx.req, ctx.key, md))
        return SecurityStatus::forCondition(SecurityCondition::kSigningFailure,
            "failed to sign certificate signing request");

    ctx.crt = X509_new();
    TU_ASSERT (ctx.crt != nullptr);

    // set version to X509v3
    X509_set_version(ctx.crt, 2);

    // set serial number
    ASN1_INTEGER_set(X509_get_serialNumber(ctx.crt), serial);

    // set issuer to CA's subject
    X509_set_issuer_name(ctx.crt, X509_get_subject_name(ctx.ca_crt));

    // set validity of certificate
    X509_gmtime_adj(X509_get_notBefore(ctx.crt), -30);
    X509_gmtime_adj(X509_get_notAfter(ctx.crt), static_cast<long>(validity.count()));

    // use the subject name from the csr without performing any validation
    X509_set_subject_name(ctx.crt, X509_REQ_get_subject_name(ctx.req));

    // use the public key from the csr
    EVP_PKEY *req_pubkey = X509_REQ_get_pubkey(ctx.req);
    X509_set_pubkey(ctx.crt, req_pubkey);
    EVP_PKEY_free(req_pubkey);

    // sign the certificate with the CA private key
    if (X509_sign(ctx.crt, ctx.ca_key, nullptr) == 0)
        return SecurityStatus::forCondition(SecurityCondition::kSigningFailure,
            "failed to sign certificate");

    std::filesystem::path pemCertificateFile = keyPairDestDirectory / keyPairFilenameStem;
    pemCertificateFile.replace_extension("crt");
    std::filesystem::path pemPrivateKeyFile = keyPairDestDirectory / keyPairFilenameStem;
    pemPrivateKeyFile.replace_extension("key");

    // write the cert
    TU_RETURN_IF_NOT_OK (write_x509_certificate(ctx.crt, pemCertificateFile));

    // write the private key
    TU_RETURN_IF_NOT_OK (write_private_key(ctx.key, pemPrivateKeyFile));

    // generation succeeded
    return CertificateKeyPair(keygen.getKeyType(), pemPrivateKeyFile, pemCertificateFile);
}

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct GenerateSelfSignedKeyPairCtx {
    EVP_PKEY *key = nullptr;
    X509 *crt = nullptr;

    ~GenerateSelfSignedKeyPairCtx() {
        if (key)
            EVP_PKEY_free(key);
        if (crt)
            X509_free(crt);
    }
};

tempo_utils::Result<tempo_security::CertificateKeyPair>
tempo_security::GenerateUtils::generate_self_signed_key_pair(
    const AbstractPrivateKeyGenerator &keygen,
    DigestId digestId,
    std::string_view organization,
    std::string_view organizationalUnit,
    std::string_view commonName,
    int serial,
    std::chrono::seconds validity,
    const std::filesystem::path &keyPairDestDirectory,
    std::string_view keyPairFilenameStem)
{
    GenerateSelfSignedKeyPairCtx ctx;

    // generate the private key
    ctx.key = keygen.generatePrivateKey();
    if (ctx.key == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to generate private key");

    // allocate the cert
    ctx.crt = X509_new();
    TU_ASSERT (ctx.crt != nullptr);

    // set version to X509v3
    X509_set_version(ctx.crt, 2);

    // set serial number
    ASN1_INTEGER_set(X509_get_serialNumber(ctx.crt), serial);

    // set validity of certificate
    X509_gmtime_adj(X509_get_notBefore(ctx.crt), -30);
    X509_gmtime_adj(X509_get_notAfter(ctx.crt), static_cast<long>(validity.count()));

    // Set the DN of the request
    auto *dn = X509_get_subject_name(ctx.crt);
    X509_NAME_add_entry_by_txt(dn, "O", MBSTRING_ASC,
        (const unsigned char*) organization.data(), organization.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "OU", MBSTRING_ASC,
        (const unsigned char*) organizationalUnit.data(), organizationalUnit.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "CN", MBSTRING_ASC,
        (const unsigned char*) commonName.data(), commonName.length(), -1, 0);

    // set v3 extensions
    //add_x509_ext(nullptr, ctx.crt, NID_netscape_comment, "example comment extension");

    // this is a self-signed cert so set issuer to the same value as subject
    X509_set_issuer_name(ctx.crt, dn);

    // set the public key
    X509_set_pubkey(ctx.crt, ctx.key);

    auto *md = get_digest(digestId);

    // sign the certificate with the CA private key
    if (X509_sign(ctx.crt, ctx.key, md) == 0)
        return SecurityStatus::forCondition(SecurityCondition::kSigningFailure,
            "failed to sign certificate");

    std::filesystem::path pemCertificateFile = keyPairDestDirectory / keyPairFilenameStem;
    pemCertificateFile.replace_extension("crt");
    std::filesystem::path pemPrivateKeyFile = keyPairDestDirectory / keyPairFilenameStem;
    pemPrivateKeyFile.replace_extension("key");

    // write the cert
    TU_RETURN_IF_NOT_OK (write_x509_certificate(ctx.crt, pemCertificateFile));

    // write the private key
    TU_RETURN_IF_NOT_OK (write_private_key(ctx.key, pemPrivateKeyFile));

    // generation succeeded
    return CertificateKeyPair(keygen.getKeyType(), pemPrivateKeyFile, pemCertificateFile);
}

/*
 * ctx holds intermediate context to be freed when it goes out of scope
 */
struct GenerateCSRKeyPairCtx {
    EVP_PKEY *key = nullptr;
    X509_REQ *req = nullptr;

    ~GenerateCSRKeyPairCtx() {
        if (key)
            EVP_PKEY_free(key);
        if (req)
            X509_REQ_free(req);
    }
};

tempo_utils::Result<tempo_security::CSRKeyPair>
tempo_security::GenerateUtils::generate_csr_key_pair(
    const AbstractPrivateKeyGenerator &keygen,
    DigestId digestId,
    std::string_view organization,
    std::string_view organizationalUnit,
    std::string_view commonName,
    const std::filesystem::path &keyPairDestDirectory,
    std::string_view keyPairFilenameStem)
{
    GenerateCSRKeyPairCtx ctx;

    // allocate the csr
    ctx.req = X509_REQ_new();
    TU_ASSERT (ctx.req != nullptr);

    // generate the private key
    ctx.key = keygen.generatePrivateKey();
    if (ctx.key == nullptr)
        return SecurityStatus::forCondition(SecurityCondition::kSecurityInvariant,
            "failed to generate private key");

    // set the DN of the request
    auto *dn = X509_REQ_get_subject_name(ctx.req);
    X509_NAME_add_entry_by_txt(dn, "O", MBSTRING_ASC,
        (const unsigned char*) organization.data(), organization.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "OU", MBSTRING_ASC,
        (const unsigned char*) organizationalUnit.data(), organizationalUnit.length(), -1, 0);
    X509_NAME_add_entry_by_txt(dn, "CN", MBSTRING_ASC,
        (const unsigned char*) commonName.data(), commonName.length(), -1, 0);

    // set the public key
    X509_REQ_set_pubkey(ctx.req, ctx.key);

    auto *md = get_digest(digestId);

    // self sign the request
    if (X509_REQ_sign(ctx.req, ctx.key, md) == 0)
        return SecurityStatus::forCondition(SecurityCondition::kSigningFailure,
            "failed to sign certificate signing request");

    std::filesystem::path pemRequestFile = keyPairDestDirectory / keyPairFilenameStem;
    pemRequestFile.replace_extension("csr");
    std::filesystem::path pemPrivateKeyFile = keyPairDestDirectory / keyPairFilenameStem;
    pemPrivateKeyFile.replace_extension("key");

    // write the csr
    TU_RETURN_IF_NOT_OK (write_x509_csr(ctx.req, pemRequestFile));

    // write the private key
    TU_RETURN_IF_NOT_OK (write_private_key(ctx.key, pemPrivateKeyFile));

    // generation succeeded
    return CSRKeyPair(keygen.getKeyType(), pemPrivateKeyFile, pemRequestFile);
}
