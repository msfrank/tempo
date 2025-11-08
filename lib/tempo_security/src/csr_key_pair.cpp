
#include "tempo_security/private_key.h"
#include "tempo_security/x509_certificate_signing_request.h"

#include <openssl/evp.h>
#include <openssl/pem.h>

#include <tempo_security/csr_key_pair.h>
#include <tempo_security/security_types.h>
#include <tempo_utils/log_message.h>

tempo_security::CSRKeyPair::CSRKeyPair()
{
}

tempo_security::CSRKeyPair::CSRKeyPair(
    KeyType keyType,
    const std::filesystem::path &pemPrivateKeyFile,
    const std::filesystem::path &pemRequestFile)
    : m_keyType(keyType),
      m_pemPrivateKeyFile(pemPrivateKeyFile),
      m_pemRequestFile(pemRequestFile)
{
    TU_ASSERT (m_keyType != KeyType::Invalid);
    TU_ASSERT (!m_pemPrivateKeyFile.empty());
    TU_ASSERT (!m_pemRequestFile.empty());
}

tempo_security::CSRKeyPair::CSRKeyPair(const CSRKeyPair &other)
    : m_keyType(other.m_keyType),
      m_pemPrivateKeyFile(other.m_pemPrivateKeyFile),
      m_pemRequestFile(other.m_pemRequestFile)
{
}

bool
tempo_security::CSRKeyPair::isValid() const
{
    return m_keyType != KeyType::Invalid;
}

tempo_security::KeyType
tempo_security::CSRKeyPair::getKeyType() const
{
    return m_keyType;
}

std::filesystem::path
tempo_security::CSRKeyPair::getPemPrivateKeyFile() const
{
    return m_pemPrivateKeyFile;
}

std::filesystem::path
tempo_security::CSRKeyPair::getPemRequestFile() const
{
    return m_pemRequestFile;
}

tempo_utils::Result<tempo_security::CSRKeyPair>
tempo_security::CSRKeyPair::load(
    const std::filesystem::path &pemPrivateKeyFile,
    const std::filesystem::path &pemRequestFile)
{
    std::shared_ptr<X509CertificateSigningRequest> csr;
    TU_ASSIGN_OR_RETURN (csr, X509CertificateSigningRequest::readFile(pemRequestFile));

    std::shared_ptr<PrivateKey> key;
    TU_ASSIGN_OR_RETURN (key, PrivateKey::readFile(pemPrivateKeyFile));

    // verify the private key matches the csr
    if (!X509_REQ_check_private_key(csr->getCertificateSigningRequest(), key->getPrivateKey()))
        return SecurityStatus::forCondition(SecurityCondition::kParseError,
            "private key {} does not match certificate signing request {}",
            pemPrivateKeyFile.string(), pemRequestFile.string());

    auto keyType = key->getKeyType();

    return CSRKeyPair(keyType, pemPrivateKeyFile, pemRequestFile);
}
