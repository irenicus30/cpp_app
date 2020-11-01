# Minimum CMake required
cmake_minimum_required(VERSION 3.10)

# Project
project(myapi)

set (CMAKE_CXX_STANDARD 14)

set(dbmgr_src "databasemanager.cpp")
set(dbmgr_hdr "databasemanager.h")

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# Protobuf-compiler
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Qt
find_package(Qt5 COMPONENTS Core Sql REQUIRED)

find_package(Threads REQUIRED)

# Proto file
get_filename_component(hw_proto "myapi.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/myapi.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/myapi.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/myapi.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/myapi.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Targets (client|server)
foreach(_target
  client server)
  add_executable(${_target} "${_target}.cpp"
    ${hw_proto_srcs}
    ${hw_grpc_srcs}
    ${dbmgr_src})
  target_link_libraries(${_target}
    Qt5::Core
    Qt5::Sql
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
