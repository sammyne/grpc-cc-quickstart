#!/bin/bash

set -e

workdir=$PWD
pki_dir=$workdir/pki/server

cmake -B build

cmake --build build --target server

#export GRPC_VERBOSITY=DEBUG
#export GRPC_TRACE=all

#./build/server $pki_dir/key.pem $pki_dir/cert.pem localhost:50051

pki_dir=$workdir/pki/non-self-signed
opts="$pki_dir/server.key $pki_dir/server.crt localhost:50051 $pki_dir/ca.crt"
./build/server $opts
