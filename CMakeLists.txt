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

set(client_srcs 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/os.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tls.cc
)

add_executable(client ${client_srcs} ${pb_srcs})

set(server_srcs 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/os.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tls.cc
)

add_executable(server ${server_srcs} ${pb_srcs})
