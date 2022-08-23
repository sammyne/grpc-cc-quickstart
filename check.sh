#!/bin/bash

set -e

workdir=$PWD
pki_dir=$workdir/pki/client

#cmake -B build

cmake --build build --target client

./build/client $pki_dir/key.pem $pki_dir/cert.pem