cmake_minimum_required(VERSION 3.16)
project(algorep)

############### EXTERNAL LIBRARIES ##############
find_package(MPI REQUIRED)
#################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER mpic++)

# Automatic Compilation Flag to Release
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

############### COMPILATION FLAGS ###############
# General compilation flags
set(CMAKE_CXX_FLAGS "-pedantic -Werror -Wall -Wextra -std=c++17 -DOMPI_SKIP_MPICXX")

# Debug compilation flags
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

# Release compilation flags
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DRELEASE -DNDEBUG -march=native")
##################################################

set(MAIN_TARGET raft)

################## DIRECTORIES ##################
include_directories(
        src
        lib)
#################################################

#################### SOURCES ####################
# LIB SOURCES
# JSON
set(JSON_SOURCES
        lib/json/json.hpp)

# Utils
set(UTILS_SOURCES
        lib/utils/clock.cc
        lib/utils/entry.cc
        lib/utils/string_utils.cc lib/utils/debugger.cc lib/utils/debugger.hh)

set(LIB_SOURCES
        ${JSON_SOURCES}
        ${UTILS_SOURCES})

# MAIN SOURCES
set(MPI_CLASSES_SOURCES
        src/mpi_classes/client.cc
        src/mpi_classes/controller.cc
        src/mpi_classes/node.cc
        src/mpi_classes/process-information.cc)

set(RPC_SOURCES
        src/rpc/rpc.cc
        src/rpc/rpc-query.cc
        src/rpc/lib-rpc.cc
        src/rpc/append-entries.cc
        src/rpc/request-vote.cc
        src/rpc/new-entry.cc
        src/rpc/search-leader.cc
        src/rpc/message.cc
        src/rpc/get-state.cc)

# Exec
set(MAIN_SOURCES
        ${MPI_CLASSES_SOURCES}
        ${RPC_SOURCES})

set(MAIN_EXEC_SOURCES
        ${MAIN_SOURCES}
        src/main.cc)
#################################################

# Main Executable
add_executable(${MAIN_TARGET}
        ${MAIN_EXEC_SOURCES}
        ${LIB_SOURCES}
        ${UTILS_SOURCES})