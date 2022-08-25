#include "xiangminli/crypto/package.h"

namespace xiangminli::crypto {

PrivateKey NewPrivateKey(EVP_PKEY *raw) {
  if (nullptr == raw) {
    raw = EVP_PKEY_new();
  }

  return PrivateKey(raw, EVP_PKEY_free);
}

}  // namespace xiangminli::crypto
