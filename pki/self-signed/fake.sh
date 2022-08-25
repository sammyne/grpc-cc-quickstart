#!/bin/bash

set -e

workdir=$PWD

md=sha256
ec_algo=ec
curve=P-256

for app in server client; do
  # -nodes: omits the password or passphrase so you can examine the certificate.
  #   It's a really bad idea to omit the password or passphrase.
  #
  # subjectAltName is treated as multiple Common Name
  #
  #subject_opt="-subj '/CN=localhost'"
  subject_opt=""
  openssl req                         \
    -batch                            \
    -days 366                         \
    -keyout $app.key                  \
    -newkey $ec_algo                  \
    -nodes                            \
    -out $app.crt                     \
    -pkeyopt ec_paramgen_curve:$curve \
    -$md                              \
    -x509                             \
    $subject_opt                      \
    -addext "subjectAltName=DNS:localhost"

  echo ""
  echo "-------------------"
  echo "certificate goes as"
  openssl x509 -noout -text -in $app.crt

  echo ""
  echo "-------------------"
  echo "private key goes as"
  openssl ec -in $app.key -noout -text
done