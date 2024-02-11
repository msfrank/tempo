
#include <openssl/conf.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

#include <tempo_security/x509_certificate.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/posix_result.h>

tempo_security::X509Certificate::X509Certificate()
    : m_x509(nullptr)
{
}

tempo_security::X509Certificate::X509Certificate(X509 *x509)
    : m_x509(x509)
{
    TU_ASSERT (m_x509 != nullptr);
}

tempo_security::X509Certificate::~X509Certificate()
{
    X509_free(m_x509);
}

bool
tempo_security::X509Certificate::isValid() const
{
    return m_x509 != nullptr;
}

long
tempo_security::X509Certificate::getVersion() const
{
    if (!isValid())
        return -1;
    return X509_get_version(m_x509);
}

long
tempo_security::X509Certificate::getSerialNumber() const
{
    if (!isValid())
        return -1;
    auto *serial = X509_get_serialNumber(m_x509);
    return ASN1_INTEGER_get(serial);
}

std::string
tempo_security::X509Certificate::getOrganization() const
{
    if (!isValid())
        return {};
    auto *dn = X509_get_subject_name(m_x509);

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
tempo_security::X509Certificate::getOrganizationalUnit() const
{
    if (!isValid())
        return {};
    auto *dn = X509_get_subject_name(m_x509);

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
tempo_security::X509Certificate::getCommonName() const
{
    if (!isValid())
        return {};
    auto *dn = X509_get_subject_name(m_x509);

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

std::chrono::time_point<std::chrono::system_clock>
tempo_security::X509Certificate::getNotValidBefore() const
{
    if (!isValid())
        return {};
    auto *notBefore = X509_get_notBefore(m_x509);
    struct tm tm;
    ASN1_TIME_to_tm(notBefore, &tm);
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::chrono::time_point<std::chrono::system_clock>
tempo_security::X509Certificate::getNotValidAfter() const
{
    if (!isValid())
        return {};
    auto *notBefore = X509_get_notBefore(m_x509);
    struct tm tm;
    ASN1_TIME_to_tm(notBefore, &tm);
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

bool
tempo_security::X509Certificate::isCertificateAuthority() const
{
    if (!isValid())
        return {};

    int critical = 0;
    auto *ret = X509_get_ext_d2i(m_x509, NID_basic_constraints, &critical, nullptr);
    if (ret == nullptr)
        return false;
    auto *bc = static_cast<BASIC_CONSTRAINTS *>(ret);
    return bc->ca > 0;
}

std::string
tempo_security::X509Certificate::toString() const
{
    auto *bio = BIO_new(BIO_s_mem());
    X509_print(bio, m_x509);
    char *data;
    auto len = BIO_get_mem_data(bio, &data);
    std::string s(data, len);
    BIO_free(bio);
    return s;
}

bool
tempo_security::X509Certificate::operator==(const X509Certificate &other) const
{
    if (m_x509 == nullptr && other.m_x509 == nullptr)
        return true;
    if (m_x509 != nullptr && other.m_x509 != nullptr)
        return X509_cmp(m_x509, other.m_x509) == 0;
    return false;
}

bool
tempo_security::X509Certificate::operator!=(const X509Certificate &other) const
{
    return !(*this == other);
}

tempo_utils::Result<std::shared_ptr<tempo_security::X509Certificate>>
tempo_security::X509Certificate::readFile(
    const std::filesystem::path &pemCertificateFile)
{
    X509 *x509 = nullptr;
    auto *bio = BIO_new_file(pemCertificateFile.c_str(), "rx");
    if (bio == nullptr)
        goto err;
    if (!PEM_read_bio_X509(bio, &x509, nullptr, nullptr))
        goto err;
    BIO_free(bio);
    return std::shared_ptr<X509Certificate>(new X509Certificate(x509));

err:
    if (bio != nullptr)
        BIO_free(bio);
    switch (errno) {
        case ENOENT:
            return tempo_utils::FileStatus::forCondition(
                tempo_utils::FileCondition::kFileNotFound,
                "certificate file {} not found", pemCertificateFile.string());
        case EACCES:
            return tempo_utils::FileStatus::forCondition(
                tempo_utils::FileCondition::kAccessDenied,
                "access denied for {}", pemCertificateFile.string());
        default:
            return tempo_utils::PosixStatus::fromError(errno);
    }
}