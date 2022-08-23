#!/bin/bash

set -e

workdir=$PWD
app=openssl

key=$workdir/key.pem
cert=$workdir/cert.pem

md=sha256
ec_algo=ec
curve=P-256

# -nodes: omits the password or passphrase so you can examine the certificate.
#   It's a really bad idea to omit the password or passphrase.
$app req                            \
  -batch                            \
  -days 366                         \
  -keyout $key                      \
  -newkey $ec_algo                  \
  -nodes                            \
  -out $cert                        \
  -pkeyopt ec_paramgen_curve:$curve \
  -$md                              \
  -x509

echo ""
echo "-------------------"
echo "certificate goes as"
$app x509 -noout -text -in $cert

echo ""
echo "-------------------"
echo "private key goes as"
$app ec -in $key -noout -text
