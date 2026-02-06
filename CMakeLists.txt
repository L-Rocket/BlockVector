cmake_minimum_required(VERSION 3.10)

project(main)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

enable_testing()
add_test(NAME test_main COMMAND test_main)

add_executable(main src/main.cpp src/example.cpp)

add_executable(test_main tests/test_main.cpp src/example.cpp)
