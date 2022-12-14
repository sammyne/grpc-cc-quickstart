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

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "grpcpp/grpcpp.h"
#include "grpcpp/security/tls_credentials_options.h"
#include "helloworld.grpc.pb.h"
#include "xiangminli/os.h"
#include "xiangminli/tls.h"

using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;
using std::vector;

using grpc::Channel;
using grpc::ChannelCredentials;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

namespace tls = xiangminli::tls;
namespace os = xiangminli::os;
namespace experimental = grpc::experimental;

shared_ptr<ChannelCredentials> NewCredentials(const char* key_path,
                                              const char* cert_path);

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  if (argc < 3) {
    cout << "[usage] key-pem-path cert-pem-path [remote-addr]" << endl;
    return -1;
  }

  const auto key_path = argv[1];
  const auto cert_path = argv[2];

  string remote_addr = "0.0.0.0:50051";
  if (argc > 3) {
    remote_addr = string(argv[3]);
  }

  auto credentials = NewCredentials(key_path, cert_path);

  // auto channel = grpc::CreateChannel(target_str, credentials);

  grpc::ChannelArguments channel_args;
  // channel_args.SetSslTargetNameOverride("localhost2");
  auto channel =
      grpc::CreateCustomChannel(remote_addr, credentials, channel_args);

  GreeterClient greeter(channel);
  std::string user("world");
  std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}

shared_ptr<ChannelCredentials> NewCredentials(const char* key_path,
                                              const char* cert_path) {
  grpc::experimental::TlsChannelCredentialsOptions opts;
  opts.set_check_call_host(false);
  opts.set_verify_server_certs(false);

  auto cert_verifier = tls::NewEnclaveCertVerifier(true);

  opts.set_certificate_verifier(cert_verifier);

  {
    string key_pem;
    auto err = os::ReadFile(key_pem, key_path);
    assert((0 == err) && "fail to read key");

    cout << "key PEM" << endl;
    cout << key_pem << endl;

    string cert_pem = "";
    err = os::ReadFile(cert_pem, cert_path);
    assert((0 == err) && "fail to read cert");

    cout << "cert PEM" << endl;
    cout << cert_pem << endl;

    experimental::IdentityKeyCertPair key_cert_pair;
    key_cert_pair.private_key = key_pem;
    key_cert_pair.certificate_chain = cert_pem;

    vector<experimental::IdentityKeyCertPair> key_cert_pairs{key_cert_pair};

    auto cert_provider =
        std::make_shared<experimental::StaticDataCertificateProvider>(
            key_cert_pairs);

    opts.set_certificate_provider(cert_provider);
  }

  return grpc::experimental::TlsCredentials(opts);
}