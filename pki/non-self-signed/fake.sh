#!/bin/bash

set -e

workdir=$PWD

ca_config=$workdir/ca.cnf
ca_key=ca.key
ca_cert=ca.crt


md=sha256
ec_algo=ec
curve=P-256

outdir=$workdir/_out

rm -rf $outdir ca-certs.db* rm serial.txt* *.csr

mkdir -p $outdir


echo "faking CA key and cert ..."

# -nodes: omits the password or passphrase so you can examine the certificate.
#   It's a really bad idea to omit the password or passphrase.
openssl req                         \
  -batch                            \
  -config $ca_config                \
  -keyout $ca_key                   \
  -newkey $ec_algo                  \
  -nodes                            \
  -out $ca_cert                     \
  -pkeyopt ec_paramgen_curve:$curve \
  -$md                              \
  -subj "/CN=sammyne"               \
  -x509

#$server x509 -noout -text -in server.crt

echo ""
echo "create CA certs DB ..."
touch ca-certs.db

for app in server client; do
  echo ""
  echo "---------------------------"
  echo "fake CSR for $app ..."
  echo ""
  openssl req                         \
    -config app.cnf                   \
    -keyout $app.key               \
    -newkey $ec_algo                  \
    -nodes                            \
    -out $app.csr                  \
    -pkeyopt ec_paramgen_curve:$curve \
    -$md                              \
    -subj "/CN=sammyne-$app"

  echo ""
  echo "---------------------------"
  echo "signing cert for $app ..."
  echo ""
  # '-md sm3' is a must in case of sm2
  openssl ca                \
    -batch                  \
    -cert $ca_cert          \
    -config ca.cnf         \
    -extensions signing_req \
    -in $app.csr         \
    -keyfile $ca_key        \
    -md $md                 \
    -notext                 \
    -out $app.crt       \
    -outdir $outdir \
    -policy signing_policy  \
    -rand_serial

  #echo ""
  #echo "------------------------"
  #echo "show server cert ..."
  #echo ""
  #$server x509 -noout -text -in $server_cert

  echo ""
  echo "---------------------"
  echo "verify cert chain for $app ..."
  echo ""
  # -untrusted is for specifying intermediate CA
  openssl verify      \
    -CAfile $ca_cert  \
    -verbose          \
    server.crt
done

rm -rf ca-certs.db* rm serial.txt* *.csr $outdir
