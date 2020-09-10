cmake_minimum_required(VERSION 3.16)
project(Dictionary)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(googletest)

include_directories(googletest/googletest/include)

add_executable(Dictionary main.cpp my_dictionary.h)
target_link_libraries(Dictionary gtest gtest_main)
