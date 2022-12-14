#!/bin/bash

set -e

workdir=$PWD
pki_dir=$workdir/pki/server

cmake -B build

cmake --build build --target server

export GRPC_VERBOSITY=DEBUG
export GRPC_TRACE=api,call_errors

./build/server $pki_dir/key.pem $pki_dir/cert.pem
