cmake_minimum_required(VERSION 3.16)
project(algorep)

set(CMAKE_CXX_STANDARD 17)

# Automatic Compiler if not defined
if (NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER mpic++)
endif()

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
set(UNIT_TESTS_TARGET unit_tests)

############### EXTERNAL LIBRARIES ##############
find_package(MPI REQUIRED)
#################################################

################## DIRECTORIES ##################
include_directories(src)
#################################################

#################### SOURCES ####################
# MAIN SOURCES
# Exec
set(EXEC_SOURCES
        src/main.cc)

# TESTS SOURCES
# Unit Tests
set(UNIT_TESTS_SOURCES
        tests/unit_tests/main.cc
        tests/unit_tests/example.cc)
#################################################

# Main Executable
add_executable(${MAIN_TARGET}
        ${EXEC_SOURCES})

# Unit Tests Custom Target
add_custom_target(unit
        COMMAND ./${UNIT_TESTS_TARGET}
        DEPENDS ${UNIT_TESTS_TARGET})

enable_testing()
add_test(${UNIT_TESTS_TARGET} ${UNIT_TESTS_TARGET})

###############################################################################
#                                 GOOGLE TEST                                 #
###############################################################################

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif ()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif ()

# Unit Tests Executable
add_executable(${UNIT_TESTS_TARGET} ${UNIT_TESTS_SOURCES})
target_link_libraries(${UNIT_TESTS_TARGET} gtest_main)