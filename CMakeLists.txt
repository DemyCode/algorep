cmake_minimum_required(VERSION 3.16)
project(algorep)

set(CMAKE_CXX_COMPILER mpic++)

find_package(MPI REQUIRED)
include_directories(src)

add_executable(raft src/main.cc src/controller.cc src/client.cc src/node.cc)