#include "xiangminli/hack.h"

#include "grpc/grpc.h"
#include "grpcpp/impl/codegen/grpc_library.h"

namespace grpc::internal {

extern std::shared_ptr<ChannelCredentials> WrapChannelCredentials(
    grpc_channel_credentials* creds);

}

namespace xiangminli::hack {

std::shared_ptr<grpc::ChannelCredentials> NewSslCredentials(
    const grpc::SslCredentialsOptions& options,
    const verify_peer_callback_fn verify_peer_callback) {
  grpc::GrpcLibraryCodegen init;

  grpc_ssl_pem_key_cert_pair pem_key_cert_pair = {
      options.pem_private_key.c_str(), options.pem_cert_chain.c_str()};

  grpc_channel_credentials* c_creds = grpc_ssl_credentials_create(
      options.pem_root_certs.empty() ? nullptr : options.pem_root_certs.c_str(),
      options.pem_private_key.empty() ? nullptr : &pem_key_cert_pair, nullptr,
      nullptr);

  return grpc::internal::WrapChannelCredentials(c_creds);
}

}  // namespace xiangminli::hack