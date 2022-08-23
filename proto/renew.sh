#!/bin/bash

workdir=$PWD

rm -f *.pb.h *.pb.cc

protoc --cpp_out=$workdir --grpc_out=$workdir helloworld.proto
