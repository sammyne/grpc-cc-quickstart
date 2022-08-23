cmake_minimum_required(VERSION 3.16.0)

project(grpc-cc-quickstart)

set(pb_dir ${CMAKE_CURRENT_SOURCE_DIR}/proto)

include(cmake/grpc.cmake)

include_directories(${pb_dir})

aux_source_directory(${pb_dir} pb_srcs)

link_libraries(
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
)

add_executable(server server.cc ${pb_srcs})

add_executable(client client.cc ${pb_srcs})
