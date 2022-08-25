cmake_minimum_required(VERSION 3.16.0)

project(grpc-cc-quickstart CXX)

set(CMAKE_CXX_STANDARD 17)

set(pb_dir ${CMAKE_CURRENT_SOURCE_DIR}/proto)

include(cmake/grpc.cmake)

#set(OPENSSL_USE_STATIC_LIBS TRUE)
#find_package(OpenSSL REQUIRED)
#message(STATUS "openssl version: ${OPENSSL_VERSION}")

include_directories(
  ${pb_dir}
  ${OPENSSL_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

aux_source_directory(${pb_dir} pb_srcs)

link_libraries(
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
)

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src srcs)
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cc)
message(STATUS "srcs: ${srcs}")

add_library(dummy STATIC ${srcs} ${pb_srcs})

foreach(app client server)
  add_executable(${app} ${CMAKE_CURRENT_SOURCE_DIR}/apps/${app}.cc)
  target_link_libraries(${app} dummy ${OPENSSL_CRYPTO_LIBRARY})
endforeach(app )
