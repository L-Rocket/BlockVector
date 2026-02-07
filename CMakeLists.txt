cmake_minimum_required(VERSION 3.14) # GTest requires somewhat newer cmake
project(BlockVector)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# --- Integrated Google Test ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# --- Old Manual Tests (Keeping them for now) ---
add_executable(test_main tests/test_main.cpp)
add_test(NAME test_main COMMAND test_main)
# ... (keeping other old tests implicitly if you want, but better to migrate)

# --- New GTest Suite ---
# We will migrate test_vector to this new suite
add_executable(unit_tests 
    tests/test_vector_gtest.cpp
)
target_link_libraries(unit_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests XML_OUTPUT_DIR ${CMAKE_BINARY_DIR}/test_results)


add_executable(test_block_stability tests/test_block_stability.cpp)
add_test(NAME test_block_stability COMMAND test_block_stability)

add_executable(test_perf_growth tests/test_perf_growth.cpp)
add_test(NAME test_perf_growth COMMAND test_perf_growth)

add_executable(test_perf_reserve tests/test_perf_reserve.cpp)
add_test(NAME test_perf_reserve COMMAND test_perf_reserve)

add_executable(test_perf_index tests/test_perf_index.cpp)
add_test(NAME test_perf_index COMMAND test_perf_index)

add_executable(test_perf_jitter tests/test_perf_jitter.cpp)
add_test(NAME test_perf_jitter COMMAND test_perf_jitter)

add_executable(test_pop_reclaim tests/test_pop_reclaim.cpp)
add_test(NAME test_pop_reclaim COMMAND test_pop_reclaim)

add_executable(test_perf_object_size tests/test_perf_object_size.cpp)
add_test(NAME test_perf_object_size COMMAND test_perf_object_size)

add_executable(test_perf_iter_vs_index tests/test_perf_iter_vs_index.cpp)
add_test(NAME test_perf_iter_vs_index COMMAND test_perf_iter_vs_index)

add_executable(test_perf_index_compare tests/test_perf_index_compare.cpp)
add_test(NAME test_perf_index_compare COMMAND test_perf_index_compare)

add_executable(main src/main.cpp)
