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
#include "xiangminli/faker.h"
#include "xiangminli/hack.h"
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
namespace faker = xiangminli::faker;
namespace hack = xiangminli::hack;
namespace os = xiangminli::os;
namespace experimental = grpc::experimental;

shared_ptr<ChannelCredentials> FakeCredentials();

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
    ClientContext ctx;

    // The actual RPC.
    Status status = stub_->SayHello(&ctx, request, &reply);

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
  string remote_addr = "0.0.0.0:50051";
  if (argc > 1) {
    remote_addr = string(argv[1]);
  }
  printf("remote-addr = %s\n", remote_addr.c_str());

  auto credentials = FakeCredentials();

  // auto channel = grpc::CreateChannel(remote_addr, credentials);

  grpc::ChannelArguments channel_args;
  channel_args.SetSslTargetNameOverride("localhost");
  auto channel =
      grpc::CreateCustomChannel(remote_addr, credentials, channel_args);

  GreeterClient greeter(channel);
  std::string user("world");
  std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}

shared_ptr<ChannelCredentials> FakeCredentials() {
  string key_pem, cert_pem;
  auto err = faker::FakeEncodedKeyAndSignedCertFromCA(key_pem, cert_pem);
  assert((0 == err) && "fake encoded key and cert");

  cout << "key PEM" << endl;
  cout << key_pem << "\n" << endl;

  cout << "cert PEM" << endl;
  cout << cert_pem << "\n" << endl;

  cout << "CA cert PEM" << endl;
  cout << faker::kCACertPEM << "\n" << endl;

  grpc::SslCredentialsOptions opts;
  opts.pem_root_certs = faker::kCACertPEM;
  opts.pem_private_key = key_pem;
  opts.pem_cert_chain = cert_pem;

  return hack::NewSslCredentials(opts, hack::DummyVerifyPeer);
}
