cmake_minimum_required(VERSION 3.16.0)

project(grpc-cc-quickstart CXX)

set(CMAKE_CXX_STANDARD 17)

set(pb_dir ${CMAKE_CURRENT_SOURCE_DIR}/proto)

include(cmake/grpc.cmake)

include_directories(${pb_dir} ${CMAKE_CURRENT_SOURCE_DIR}/include)

aux_source_directory(${pb_dir} pb_srcs)

link_libraries(
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
)

set(src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB lib_srcs ${src_dir}/*.cc)

add_library(dummy STATIC ${lib_srcs} ${pb_srcs})

add_executable(client ${src_dir}/bin/client.cc)
target_link_libraries(client dummy)

add_executable(server ${src_dir}/bin/server.cc)
target_link_libraries(server dummy)
