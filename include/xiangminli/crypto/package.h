#pragma once

#include <memory>

#include "openssl/evp.h"

namespace xiangminli::crypto {

using PrivateKey = std::shared_ptr<EVP_PKEY>;

PrivateKey NewPrivateKey(EVP_PKEY *raw = nullptr);

}  // namespace xiangminli::crypto
