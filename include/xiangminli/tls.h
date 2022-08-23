#pragma once

#include <functional>
#include <memory>

#include "grpcpp/security/tls_certificate_verifier.h"

namespace xiangminli::tls {

using grpc::experimental::CertificateVerifier;
using grpc::experimental::ExternalCertificateVerifier;
using grpc::experimental::TlsCustomVerificationCheckRequest;

class EnclaveCertVerifier : public ExternalCertificateVerifier {
 public:
  EnclaveCertVerifier(bool for_debug);
  ~EnclaveCertVerifier();

  bool Verify(TlsCustomVerificationCheckRequest *request,
              std::function<void(grpc::Status)> callback,
              grpc::Status *sync_status) override;

  void Cancel(TlsCustomVerificationCheckRequest *request) override;

 private:
  bool for_debug_;
};

std::shared_ptr<CertificateVerifier> NewEnclaveCertVerifier(bool for_debug);

}  // namespace xiangminli::tls