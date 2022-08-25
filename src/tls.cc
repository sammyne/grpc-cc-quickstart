#include "xiangminli/tls.h"

#include <cstdio>
#include <iostream>

namespace xiangminli::tls {

using namespace std;

using grpc::Status;
using grpc::StatusCode;

EnclaveCertVerifier::EnclaveCertVerifier(bool for_debug) {
  this->for_debug_ = for_debug;
}

EnclaveCertVerifier::~EnclaveCertVerifier() {}

bool EnclaveCertVerifier::Verify(TlsCustomVerificationCheckRequest *request,
                                 std::function<void(grpc::Status)> callback,
                                 grpc::Status *sync_status) {
  printf("hello from xml\n");
  cout << "peer cert PEM" << endl;
  cout << request->peer_cert() << endl;
  cout << "peer cert chain PEM" << endl;
  cout << request->peer_cert_full_chain() << endl;
  cout << "target name = " << request->target_name() << endl;

  if (this->for_debug_) {
    printf("pass through any server cert for debug mode\n");
    return true;
  }

  Status status(StatusCode::UNAUTHENTICATED, "bad cert");

  *sync_status = status;

  return true;  // true means sync
}

void EnclaveCertVerifier::Cancel(TlsCustomVerificationCheckRequest *request) {}

std::shared_ptr<CertificateVerifier> NewEnclaveCertVerifier(bool for_debug) {
  return ExternalCertificateVerifier::Create<EnclaveCertVerifier>(for_debug);
}

}  // namespace xiangminli::tls