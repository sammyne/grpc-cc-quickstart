#pragma once

#include <memory>

#include "grpcpp/security/credentials.h"

namespace xiangminli::hack {

using verify_peer_callback_fn = int (*)(const char *target_name,
                                        const char *peer_pem, void *userdata);

int DummyVerifyPeer(const char *target_name, const char *peer_pem,
                    void *userdata);

std::shared_ptr<grpc::ChannelCredentials> NewSslCredentials(
    const grpc::SslCredentialsOptions &options,
    const verify_peer_callback_fn verify_peer_callback = nullptr);

}  // namespace xiangminli::hack