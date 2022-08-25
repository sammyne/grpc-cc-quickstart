/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "grpc/grpc_security.h"
#include "grpcpp/ext/proto_server_reflection_plugin.h"
#include "grpcpp/grpcpp.h"
#include "grpcpp/health_check_service_interface.h"
#include "grpcpp/security/credentials.h"
#include "grpcpp/security/tls_certificate_provider.h"
#include "grpcpp/security/tls_credentials_options.h"
#include "helloworld.grpc.pb.h"
#include "xiangminli/faker.h"
#include "xiangminli/os.h"
#include "xiangminli/tls.h"

using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;
using std::vector;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCredentials;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

namespace os = xiangminli::os;
namespace faker = xiangminli::faker;
namespace tls = xiangminli::tls;
namespace experimental = grpc::experimental;

shared_ptr<ServerCredentials> FakeCredentials();

shared_ptr<ServerCredentials> NewCredentials(const char *key_path,
                                             const char *cert_path,
                                             const char *root_ca_cert_path);

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext *ctx, const HelloRequest *request,
                  HelloReply *reply) override {
    /*{
      cout << "peer uri: " << ctx->peer() << endl;
      auto auth_ctx = ctx->auth_context();
      printf("authenticated: %d\n", auth_ctx->IsPeerAuthenticated());
      for (const auto &v : auth_ctx->GetPeerIdentity()) {
        cout << "peer id: " << v << endl;
      }
      // printf()
      cout << "id prop name: " << auth_ctx->GetPeerIdentityPropertyName()
           << endl;

      for (auto i = auth_ctx->begin(); i != auth_ctx->end(); i++) {
        auto v = *i;
        cout << v.first << ": " << v.second << endl;
      }
    }
    */

    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
};

int main(int argc, char **argv) {
  if (argc < 3) {
    cout << "missing key-path and cert-path" << endl;
    return -1;
  }
  auto key_path = argv[1];
  auto cert_path = argv[2];

  string listening_addr = "0.0.0.0:50051";
  if (argc > 3) {
    listening_addr = string(argv[3]);
  }

  char *root_ca_cert_path = nullptr;
  if (argc > 4) {
    root_ca_cert_path = argv[4];
  }

  GreeterServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  // auto credentials = NewCredentials(key_path, cert_path, root_ca_cert_path);
  auto credentials = FakeCredentials();

  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(listening_addr, credentials);
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Register "service" as the instance through which we'll communicate with
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << listening_addr << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();

  return 0;
}

shared_ptr<ServerCredentials> FakeCredentials() {
  string key_pem, cert_pem;
  auto err = faker::FakeEncodedKeyAndSignedCertFromCA(key_pem, cert_pem);
  assert((0 == err) && "fake encoded key and cert");

  cout << "key PEM" << endl;
  cout << key_pem << "\n" << endl;

  cout << "cert PEM" << endl;
  cout << cert_pem << "\n" << endl;

  cout << "CA cert PEM" << endl;
  cout << faker::kCACertPEM << "\n" << endl;

  experimental::IdentityKeyCertPair key_cert_pair;
  key_cert_pair.private_key = key_pem;
  key_cert_pair.certificate_chain = cert_pem;

  vector<experimental::IdentityKeyCertPair> key_cert_pairs{key_cert_pair};

  auto cert_provider =
      std::make_shared<experimental::StaticDataCertificateProvider>(
          faker::kCACertPEM, key_cert_pairs);
  experimental::TlsServerCredentialsOptions opts(cert_provider);
  // opts.set_check_call_host(false);
  opts.watch_identity_key_cert_pairs();  // magic line to avoid segfault

  // auto cert_req_type = grpc_ssl_client_certificate_request_type::
  //     GRPC_SSL_DONT_REQUEST_CLIENT_CERTIFICATE;
  auto cert_req_type = grpc_ssl_client_certificate_request_type::
      GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_BUT_DONT_VERIFY;

  opts.set_cert_request_type(cert_req_type);

  auto cert_verifier = tls::NewEnclaveCertVerifier(true);
  opts.set_certificate_verifier(cert_verifier);

  return experimental::TlsServerCredentials(opts);
}

shared_ptr<ServerCredentials> NewCredentials(const char *key_path,
                                             const char *cert_path,
                                             const char *root_ca_cert_path) {
  string key_pem = "";

  auto err = os::ReadFile(key_pem, key_path);
  assert((0 == err) && "fail to read key");

  cout << "key PEM" << endl;
  cout << key_pem << endl;

  string cert_pem = "";
  err = os::ReadFile(cert_pem, cert_path);
  assert((0 == err) && "fail to read cert");

  cout << "cert PEM" << endl;
  cout << cert_pem << endl;

  string root_ca_cert_pem;
  if (nullptr != root_ca_cert_path) {
    err = os::ReadFile(root_ca_cert_pem, root_ca_cert_path);
    assert((0 == err) && "fail to read CA cert");

    cout << "root CA cert PEM" << endl;
    cout << root_ca_cert_pem << endl;
  }

  /*
  // v1
  // there is no custom peer verify callback available
  grpc::SslServerCredentialsOptions::PemKeyCertPair key_cert{key_pem, cert_pem};

  auto cert_req_type = grpc_ssl_client_certificate_request_type::
      GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_BUT_DONT_VERIFY;
  grpc::SslServerCredentialsOptions opts(cert_req_type);

  opts.pem_key_cert_pairs.push_back(key_cert);

  return grpc::SslServerCredentials(opts);
  */

  experimental::IdentityKeyCertPair key_cert_pair;
  key_cert_pair.private_key = key_pem;
  key_cert_pair.certificate_chain = cert_pem;

  vector<experimental::IdentityKeyCertPair> key_cert_pairs{key_cert_pair};

  auto cert_provider =
      std::make_shared<experimental::StaticDataCertificateProvider>(
          root_ca_cert_pem, key_cert_pairs);
  experimental::TlsServerCredentialsOptions opts(cert_provider);
  // opts.set_check_call_host(false);
  opts.watch_identity_key_cert_pairs();  // magic line to avoid segfault

  // auto cert_req_type = grpc_ssl_client_certificate_request_type::
  //     GRPC_SSL_DONT_REQUEST_CLIENT_CERTIFICATE;
  auto cert_req_type = grpc_ssl_client_certificate_request_type::
      GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_BUT_DONT_VERIFY;

  opts.set_cert_request_type(cert_req_type);

  auto cert_verifier = tls::NewEnclaveCertVerifier(true);
  opts.set_certificate_verifier(cert_verifier);

  return experimental::TlsServerCredentials(opts);
}