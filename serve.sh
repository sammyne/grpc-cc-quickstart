#!/bin/bash

set -e

workdir=$PWD
#pki_dir=$workdir/pki/non-self-signed
pki_dir=$workdir/pki/self-signed

cmake -B build

cmake --build build --target server

#export GRPC_VERBOSITY=DEBUG
#export GRPC_VERBOSITY=INFO
export GRPC_VERBOSITY=TRACE
#export GRPC_TRACE=all

#./build/server $pki_dir/key.pem $pki_dir/cert.pem localhost:50051

#opts="$pki_dir/server.key $pki_dir/server.crt localhost:50051 $pki_dir/ca.crt"
opts="$pki_dir/server.key $pki_dir/server.crt localhost:50051"
./build/server $opts
