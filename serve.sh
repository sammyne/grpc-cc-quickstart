#!/bin/bash

workdir=$PWD
pki_dir=$workdir/pki/server

#cmake -B build

#cmake --build build --target server

./build/server $pki_dir/key.pem $pki_dir/cert.pem
