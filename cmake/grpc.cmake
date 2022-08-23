
#set(Protobuf_DIR /grpc/lib/cmake/protobuf)

#set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED
  PATHS /grpc/lib/cmake/protobuf
  NO_DEFAULT_PATH
)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)

# absl is required by grpc
find_package(absl CONFIG REQUIRED
  PATHS /grpc/lib/cmake/absl
  NO_DEFAULT_PATH
)

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED
  PATHS /grpc/lib/cmake/grpc
  NO_DEFAULT_PATH
)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
