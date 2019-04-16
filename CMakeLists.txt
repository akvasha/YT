cmake_minimum_required(VERSION 3.11)
project(YT)

include_directories(include)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Werror")

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

# to find *.bp.h files
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER proto/messages.proto)
add_library(proto ${PROTO_HEADER} ${PROTO_SRC})

add_executable(YT main.cpp)
target_link_libraries(YT folly)
target_link_libraries(YT iberty)
target_link_libraries(YT proto ${PROTOBUF_LIBRARY})