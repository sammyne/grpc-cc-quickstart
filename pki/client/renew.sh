#!/bin/bash

set -e

# ref: https://stackoverflow.com/a/10176685/10878967
openssl req                         \
  -batch                            \
  -days 366                         \
  -keyout key.pem                   \
  -newkey ec                        \
  -nodes                            \
  -out cert.pem                     \
  -pkeyopt ec_paramgen_curve:P-256  \
  -sha256                           \
  -subj "/CN=client"                \
  -x509

echo ""
echo "key goes as"
echo "---"
openssl ec -in key.pem -noout -text

echo ""
echo "cert goes as"
echo "---"
openssl x509 -noout -text -in cert.pem
