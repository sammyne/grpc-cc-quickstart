#pragma once

#include <memory>

#include "openssl/ec.h"
#include "openssl/evp.h"
#include "xiangminli/crypto/package.h"

namespace xiangminli::crypto::secp256r1 {

int GenerateKey(PrivateKey &out);

}  // namespace xiangminli::crypto::secp256r1
