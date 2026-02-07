cmake_minimum_required(VERSION 3.14)
project(BlockVector VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. Define the Interface Library (Header-only) ---
# This allows users to just `target_link_libraries(myapp PRIVATE BlockVector)`
add_library(BlockVector INTERFACE)
target_include_directories(BlockVector INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# --- 2. Testing & Development (Only runs if this is the main project) ---
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(STATUS "Building BlockVector tests (Standalone Mode)")

    include_directories(include)

    # --- Coverage Flags (Only for testing) ---
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      add_compile_options(--coverage)
      add_link_options(--coverage)
    endif()

    # --- Integrated Google Test ---
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)

    # --- Main Test Suite ---
    add_executable(unit_tests 
        tests/test_vector_gtest.cpp
        tests/test_init_list.cpp
        tests/test_traits.cpp
    )
    target_link_libraries(unit_tests GTest::gtest_main)
    gtest_discover_tests(unit_tests XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/test_results)

    # --- Legacy/Perf Tests (Optional) ---
    # You can keep adding your other performance tests here...
    add_executable(test_perf_iter_vs_index tests/test_perf_iter_vs_index.cpp)
    # etc...
    
    # Example src/main.cpp build
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
        add_executable(demo_main src/main.cpp)
    endif()

else()
    message(STATUS "BlockVector integrated as a subproject (Tests disabled)")
endif()