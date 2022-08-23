#!/bin/bash

set -e

workdir=$PWD
pki_dir=$workdir/pki/client

#cmake -B build

cmake --build build --target client

#export GRPC_VERBOSITY=DEBUG
#export GRPC_TRACE=all

./build/client $pki_dir/key.pem $pki_dir/cert.pem