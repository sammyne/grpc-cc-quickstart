#!/bin/bash

set -e

# ref: https://stackoverflow.com/a/10176685/10878967
openssl req -x509                   \
  -newkey ec                        \
  -keyout key.pem                   \
  -nodes                            \
  -out cert.pem                     \
  -sha256                           \
  -days 366                         \
  -pkeyopt ec_paramgen_curve:P-256  \
  -subj "/CN=client"

echo ""
echo "key goes as"
echo "---"
openssl ec -in key.pem -noout -text

echo ""
echo "cert goes as"
echo "---"
openssl x509 -noout -text -in cert.pem
