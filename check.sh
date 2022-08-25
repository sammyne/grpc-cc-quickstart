#!/bin/bash

set -e

workdir=$PWD
#pki_dir=$workdir/pki/non-self-signed
pki_dir=$workdir/pki/self-signed

#cmake -B build

cmake --build build --target client

#export GRPC_VERBOSITY=DEBUG
#export GRPC_TRACE=all

#opts="$pki_dir/client.key $pki_dir/client.crt localhost:50051 $pki_dir/ca.crt"
opts="$pki_dir/client.key $pki_dir/client.crt localhost:50051"

./build/client $opts