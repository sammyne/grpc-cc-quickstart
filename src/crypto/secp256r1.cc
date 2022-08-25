#include "xiangminli/crypto/secp256r1.h"

#include "xiangminli/ministd.h"

namespace ministd = xiangminli::ministd;

namespace xiangminli::crypto::secp256r1 {

using std::shared_ptr;

int GenerateKey(PrivateKey &out) {
  auto k = ministd::NewUniquePtr(EC_KEY_new_by_curve_name(NID_X9_62_prime256v1),
                                 EC_KEY_free);
  if (!k) {
    return 1;
  }

  if (1 != EC_KEY_generate_key(k.get())) {
    return 2;
  }

  auto tmp = ministd::NewSharedPtr(EVP_PKEY_new(), EVP_PKEY_free);
  if (1 != EVP_PKEY_assign_EC_KEY(tmp.get(), k.get())) {
    return 3;
  }
  k.release();

  out = tmp;

  return 1;
}

}  // namespace xiangminli::crypto::secp256r1