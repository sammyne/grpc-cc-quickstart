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
namespace tls = xiangminli::tls;
namespace experimental = grpc::experimental;

shared_ptr<ServerCredentials> NewCredentials(const char *key_path,
                                             const char *cert_path);

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext *context, const HelloRequest *request,
                  HelloReply *reply) override {
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

  GreeterServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  auto credentials = NewCredentials(key_path, cert_path);

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

shared_ptr<ServerCredentials> NewCredentials(const char *key_path,
                                             const char *cert_path) {
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

  /*
    // v1
    grpc::SslServerCredentialsOptions::PemKeyCertPair key_cert{key_pem,
    cert_pem};

    grpc::SslServerCredentialsOptions opts;
    // grpc::SslServerCredentialsOptions opts(
    //     GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY);

    opts.pem_key_cert_pairs.push_back(key_cert);

    return grpc::SslServerCredentials(opts);
    */

  experimental::IdentityKeyCertPair key_cert_pair;
  key_cert_pair.private_key = key_pem;
  key_cert_pair.certificate_chain = cert_pem;

  vector<experimental::IdentityKeyCertPair> key_cert_pairs{key_cert_pair};

  auto cert_provider =
      std::make_shared<experimental::StaticDataCertificateProvider>(
          key_cert_pairs);
  experimental::TlsServerCredentialsOptions opts(cert_provider);
  // opts.set_check_call_host(false);
  opts.watch_identity_key_cert_pairs();  // magic line to avoid segfault

  opts.set_cert_request_type(
      grpc_ssl_client_certificate_request_type::
          GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_BUT_DONT_VERIFY);

  auto cert_verifier = tls::NewEnclaveCertVerifier(false);
  opts.set_certificate_verifier(cert_verifier);

  return experimental::TlsServerCredentials(opts);
}