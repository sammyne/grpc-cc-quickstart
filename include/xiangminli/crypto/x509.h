#pragma once

#include <memory>
#include <string>

#include "openssl/x509v3.h"
#include "xiangminli/crypto/package.h"

namespace xiangminli::crypto::x509 {

namespace crypto = ::xiangminli::crypto;

using Certificate = std::shared_ptr<X509>;

Certificate NewCertificate(X509 *raw = nullptr);

int MarshalCertificateToPEM(const Certificate &c, std::string &out);

int MarshalPKCS8PrivateKeyToPEM(const crypto::PrivateKey &privkey,
                                std::string &out);

int ParseCertificateFromPEM(const char *pem, Certificate &out);

int ParsePKCS8PrivateKeyFromPEM(const char *pem, crypto::PrivateKey &out);

}  // namespace xiangminli::crypto::x509
