#include "xiangminli/crypto/x509.h"

#include <cstring>

#include "openssl/pem.h"
#include "xiangminli/ministd.h"

namespace xiangminli::crypto::x509 {

namespace ministd = ::xiangminli::ministd;

using std::string;

Certificate NewCertificate(X509 *raw) {
  if (nullptr == raw) {
    raw = X509_new();
  }

  auto out = Certificate(raw, [](auto v) {
    if (nullptr != v) {
      X509_free(v);
    }
  });

  return out;
}

int MarshalCertificateToPEM(const Certificate &c, std::string &out) {
  auto buf = ministd::NewUniquePtr(BIO_new(BIO_s_mem()), BIO_free);
  if (!buf) {
    return 1;
  }

  if (auto err = PEM_write_bio_X509(buf.get(), c.get()); 1 != err) {
    return (err << 8) | 0x02;
  }

  BUF_MEM *mem = nullptr;
  BIO_get_mem_ptr(buf.get(), &mem);
  if (!mem || !mem->data | (mem->length <= 0)) {
    return 3;
  }

  out = string(mem->data, mem->length);

  return 0;
}

int MarshalPKCS8PrivateKeyToPEM(const crypto::PrivateKey &privkey,
                                std::string &out) {
  auto buf = ministd::NewUniquePtr(BIO_new(BIO_s_mem()), BIO_free);
  if (!buf) {
    return 1;
  }

  if (auto err = PEM_write_bio_PrivateKey(buf.get(), privkey.get(), nullptr,
                                          nullptr, 0, nullptr, nullptr);
      1 != err) {
    return (err << 8) | 0x02;
  }

  auto n = BIO_number_written(buf.get());

  string tmp(n, 0);
  if (auto ell = BIO_read(buf.get(), tmp.data(), tmp.length()); ell != n) {
    return 3;
  }

  out.swap(tmp);

  return 0;
}

int ParseCertificateFromPEM(const char *pem, Certificate &out) {
  auto buf = ministd::NewUniquePtr(BIO_new(BIO_s_mem()), BIO_free);
  if (!buf) {
    return 1;
  }

  auto pem_len = strlen(pem);

  size_t written = 0;
  if (auto err = BIO_write_ex(buf.get(), pem, pem_len, &written); 1 != err) {
    return (err << 8) | 0x02;
  }

  auto tmp = NewCertificate(PEM_read_bio_X509_AUX(buf.get(), NULL, NULL, NULL));
  if (!tmp) {
    return 3;
  }

  out = tmp;

  return 0;
}

int ParsePKCS8PrivateKeyFromPEM(const char *pem, crypto::PrivateKey &out) {
  auto buf = ministd::NewUniquePtr(BIO_new(BIO_s_mem()), BIO_free);

  auto pem_len = strlen(pem);

  if (auto err = BIO_write(buf.get(), pem, pem_len); err <= 1) {
    return 1;
  }

  auto k = PEM_read_bio_PrivateKey(buf.get(), nullptr, nullptr, nullptr);
  if (!k) {
    return 2;
  }
  out = crypto::NewPrivateKey(k);

  return 0;
}

}  // namespace xiangminli::crypto::x509
