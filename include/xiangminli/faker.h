#pragma once

#include <memory>
#include <string>

#include "openssl/ec.h"
#include "openssl/evp.h"
#include "xiangminli/crypto/package.h"
#include "xiangminli/crypto/secp256r1.h"
#include "xiangminli/crypto/x509.h"

namespace xiangminli::faker {

namespace x509 = ::xiangminli::crypto::x509;
namespace secp256r1 = ::xiangminli::crypto::secp256r1;
namespace crypto = ::xiangminli::crypto;

extern const std::string kCACertPEM;
extern const std::string kCAPrivKeyPEM;

int FakeEncodedKeyAndSignedCertFromCA(std::string &pkcs8_privkey_pem,
                                      std::string &cert_pem);

int FakeKeyAndSignedCertFromCA(crypto::PrivateKey &privkey,
                               x509::Certificate &cert);

}  // namespace xiangminli::faker
