cmake_minimum_required(VERSION 3.16)
project(algorep)

############### EXTERNAL LIBRARIES ##############
find_package(MPI REQUIRED)
#################################################

set(CMAKE_CXX_COMPILER mpic++)

# Automatic Compilation Flag to Release
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

############### COMPILATION FLAGS ###############
# General compilation flags
set(CMAKE_CXX_FLAGS "-pedantic -Werror -Wall -Wextra -std=c++17")

# Debug compilation flags
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

# Release compilation flags
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DRELEASE -DNDEBUG -march=native")
##################################################

set(MAIN_TARGET raft)

################## DIRECTORIES ##################
include_directories(src)
#################################################

#################### SOURCES ####################
# MAIN SOURCES
# Exec
set(EXEC_SOURCES
        src/main.cc
        src/mpiclasses/client.cc
        src/mpiclasses/controller.cc
        src/mpiclasses/node.cc
        src/message/message.cc)
#################################################

# Main Executable
add_executable(${MAIN_TARGET}
        ${EXEC_SOURCES})