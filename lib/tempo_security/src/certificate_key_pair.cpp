
#include "tempo_security/private_key.h"
#include "tempo_security/x509_certificate.h"

#include <openssl/evp.h>
#include <openssl/pem.h>

#include <tempo_security/certificate_key_pair.h>
#include <tempo_utils/log_message.h>

tempo_security::CertificateKeyPair::CertificateKeyPair()
{
}

tempo_security::CertificateKeyPair::CertificateKeyPair(
    KeyType keyType,
    const std::filesystem::path &pemPrivateKeyFile,
    const std::filesystem::path &pemCertificateFile)
    : m_keyType(keyType),
      m_pemPrivateKeyFile(pemPrivateKeyFile),
      m_pemCertificateFile(pemCertificateFile)
{
    TU_ASSERT (m_keyType != KeyType::Invalid);
    TU_ASSERT (!m_pemPrivateKeyFile.empty());
    TU_ASSERT (!m_pemCertificateFile.empty());
}

tempo_security::CertificateKeyPair::CertificateKeyPair(const CertificateKeyPair &other)
    : m_keyType(other.m_keyType),
      m_pemPrivateKeyFile(other.m_pemPrivateKeyFile),
      m_pemCertificateFile(other.m_pemCertificateFile)
{
}

bool
tempo_security::CertificateKeyPair::isValid() const
{
    return m_keyType != KeyType::Invalid;
}

tempo_security::KeyType
tempo_security::CertificateKeyPair::getKeyType() const
{
    return m_keyType;
}

std::filesystem::path
tempo_security::CertificateKeyPair::getPemPrivateKeyFile() const
{
    return m_pemPrivateKeyFile;
}

std::filesystem::path
tempo_security::CertificateKeyPair::getPemCertificateFile() const
{
    return m_pemCertificateFile;
}

tempo_utils::Result<tempo_security::CertificateKeyPair>
tempo_security::CertificateKeyPair::load(
    const std::filesystem::path &pemPrivateKeyFile,
    const std::filesystem::path &pemCertificateFile)
{
    std::shared_ptr<X509Certificate> crt;
    TU_ASSIGN_OR_RETURN (crt, X509Certificate::readFile(pemCertificateFile));

    std::shared_ptr<PrivateKey> key;
    TU_ASSIGN_OR_RETURN (key, PrivateKey::readFile(pemPrivateKeyFile));

    // verify the private key matches the certificate
    if (!X509_check_private_key(crt->getCertificate(), key->getPrivateKey()))
        return SecurityStatus::forCondition(SecurityCondition::kParseError,
            "private key {} does not match certificate {}",
                pemPrivateKeyFile.string(), pemCertificateFile.string());

    auto keyType = key->getKeyType();

    return CertificateKeyPair(keyType, pemPrivateKeyFile, pemCertificateFile);
}