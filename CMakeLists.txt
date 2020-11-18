project('algorep')
cmake_minimum_required(VERSION 3.16)

find_package(MPI REQUIRED)

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)

add_executable(main main.c)