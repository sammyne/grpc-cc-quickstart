#include "xiangminli/faker.h"

#include <cassert>
#include <utility>
#include <vector>

namespace xiangminli::faker {

using std::pair;
using std::string;
using std::vector;

using crypto::PrivateKey;
using x509::Certificate;

const std::string kCACertPEM = R"(
-----BEGIN CERTIFICATE-----
MIIBiTCCAS+gAwIBAgIUVe4tVuMG4XMzQ3EOyNkIomgpzeEwCgYIKoZIzj0EAwIw
EjEQMA4GA1UEAwwHc2FtbXluZTAeFw0yMjA4MjQwMTI5MThaFw0yMjA5MjMwMTI5
MThaMBIxEDAOBgNVBAMMB3NhbW15bmUwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNC
AAQSBhNvSRZwf6QBveQ1SB9AHGvy+8l5CqPihcfymUh2zBov6j8YjvO3bMx1QXD2
KVAbAxD1K7sXOfaNJWGaZVSyo2MwYTAdBgNVHQ4EFgQUL0zHNftPjjczo5dktlqn
ypAbtBYwHwYDVR0jBBgwFoAUL0zHNftPjjczo5dktlqnypAbtBYwEgYDVR0TAQH/
BAgwBgEB/wIBATALBgNVHQ8EBAMCAQYwCgYIKoZIzj0EAwIDSAAwRQIgY4zW73XA
iirQ3z7yqdbSqAbhK4LvHhqp0jUzXn0iMn4CIQDHaQaHMM6qSfoGDkjWn5L2uWGF
Dh0QmKB667cn5q9EZA==
-----END CERTIFICATE-----
)";

const std::string kCAPrivKeyPEM = R"(
-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg/6sQ99vjjsbxQNtk
1KTkSUW+Dw15Gy92y7OfKEWh8mChRANCAAQSBhNvSRZwf6QBveQ1SB9AHGvy+8l5
CqPihcfymUh2zBov6j8YjvO3bMx1QXD2KVAbAxD1K7sXOfaNJWGaZVSy
-----END PRIVATE KEY-----
)";

namespace internal {

// Add extension using V3 code: we can set the config file as NULL because we
// wont reference any other sections.
int AddExtensionsToCert(X509 *cert, vector<pair<int, string>> extensions,
                        X509 *issuer = nullptr) {
  if (nullptr == issuer) {
    issuer = cert;
  }

  X509V3_CTX ctx;
  // This sets the 'context' of the extensions. No configuration database
  X509V3_set_ctx_nodb(&ctx);

  X509V3_set_ctx(&ctx, issuer, cert, NULL, NULL, 0);

  for (size_t i = 0; i < extensions.size(); ++i) {
    auto nid = extensions[i].first;
    auto value = extensions[i].second.c_str();

    auto ext = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
    if (!ext) {
      return (i << 8) | 1;
    }

    X509_add_ext(cert, ext, -1);
    X509_EXTENSION_free(ext);
  }

  return 0;
}

Certificate MustParseCertificateFromPEM(const char *pem) {
  Certificate out = nullptr;
  auto err = x509::ParseCertificateFromPEM(pem, out);
  assert((0 == err) && "parse cert failed");
  return out;
}

PrivateKey MustParsePKCS8PrivateKeyFromPEM(const char *pem) {
  PrivateKey out = nullptr;
  auto err = x509::ParsePKCS8PrivateKeyFromPEM(pem, out);
  assert((0 == err) && "parse private key");
  return out;
}

}  // namespace internal

const Certificate kCACert =
    internal::MustParseCertificateFromPEM(kCACertPEM.c_str());

const PrivateKey kCAPrivKey =
    internal::MustParsePKCS8PrivateKeyFromPEM(kCAPrivKeyPEM.c_str());

int FakeEncodedKeyAndSignedCertFromCA(std::string &pkcs8_privkey_pem,
                                      std::string &cert_pem) {
  PrivateKey privkey = nullptr;
  Certificate cert = nullptr;
  if (auto err = FakeKeyAndSignedCertFromCA(privkey, cert); 0 != err) {
    return (err << 8) | 0x01;
  }

  if (auto err = x509::MarshalPKCS8PrivateKeyToPEM(privkey, pkcs8_privkey_pem);
      0 != err) {
    return (err << 8) | 0x02;
  }

  if (auto err = x509::MarshalCertificateToPEM(cert, cert_pem); 0 != err) {
    return (err << 8) | 0x03;
  }

  return 0;
}

int FakeKeyAndSignedCertFromCA(crypto::PrivateKey &privkey_,
                               x509::Certificate &cert_) {
  const int kSerial = 123;

  PrivateKey privkey = nullptr;
  if (auto err = secp256r1::GenerateKey(privkey); 0 != err) {
    return (err << 8) | 0x01;
  }

  auto cert = x509::NewCertificate();
  if (!cert) {
    return 2;
  }
  auto cert_ptr = cert.get();

  // v3 is denoted as 2
  X509_set_version(cert.get(), 2);
  ASN1_INTEGER_set(X509_get_serialNumber(cert.get()), kSerial);
  X509_gmtime_adj(X509_get_notBefore(cert.get()), 0);
  // one year
  X509_gmtime_adj(X509_get_notAfter(cert.get()), (int64_t)60 * 60 * 24 * 366);
  X509_set_pubkey(cert.get(), privkey.get());

  auto ca_cert = kCACert.get();

  {
    auto issuer = X509_get_subject_name(ca_cert);
    auto issuer_str = X509_NAME_oneline(issuer, nullptr, 0);
    printf("issuer = %s\n", issuer_str);
    OPENSSL_free(issuer_str);
    X509_set_issuer_name(cert.get(), issuer);
  }

  vector<pair<int, string>> extensions = {
      std::make_pair<int, string>(NID_key_usage,
                                  "critical,digitalSignature,keyEncipherment"),
      std::make_pair<int, string>(NID_subject_key_identifier, "hash"),
      std::make_pair<int, string>(NID_authority_key_identifier, "keyid:always"),
  };
  if (auto err =
          internal::AddExtensionsToCert(cert.get(), extensions, ca_cert)) {
    return (err << 8) | 0x03;
  }

  if (!X509_sign(cert_ptr, kCAPrivKey.get(), EVP_sha256())) {
    return 4;
  }

  privkey_ = privkey;
  cert_ = cert;

  return 0;
}

}  // namespace xiangminli::faker
