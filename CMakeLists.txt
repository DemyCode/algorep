project('algorep')
cmake_minimum_required(VERSION 3.16)

find_package(MPI REQUIRED)
# find_package(GTest REQUIRED)

SET(CMAKE_CXX_COMPILER mpic++)

add_executable(main src/main.cpp)

# enable_testing()
# add_executable(maintest tests/test.cpp)
# target_link_libraries(maintest GTest::GTest GTest::Main)
# add_test(maintest maintest)