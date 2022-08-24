#!/bin/bash

set -e

workdir=$PWD
pki_dir=$workdir/pki/client

#cmake -B build

cmake --build build --target client

#export GRPC_VERBOSITY=DEBUG
#export GRPC_TRACE=all

pki_dir=$workdir/pki
opts="$pki_dir/client.key $pki_dir/client.crt localhost:50051 $pki_dir/ca.crt"

./build/client $opts