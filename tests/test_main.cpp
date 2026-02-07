#include "BlockVector.hpp"

#include <chrono>
#include <iostream>
#include <vector>

namespace {
constexpr size_t kCount = 1000000;

template <typename Func>
double time_ms(Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    return elapsed.count();
}

template <typename Vec>
size_t read_sum(Vec& v) {
    volatile size_t sum = 0;
    for (size_t i = 0; i < v.size(); i += 64) {
        sum += static_cast<size_t>(v[i]);
    }
    return sum;
}
}

int main() {
    std::cout << "Benchmark size: " << kCount << "\n";

    BlockVector<int> chunked;
    std::vector<int> standard;

    double chunked_push = time_ms([&]() {
        for (size_t i = 0; i < kCount; ++i) {
            chunked.push_back(static_cast<int>(i));
        }
    });

    double std_push = time_ms([&]() {
        for (size_t i = 0; i < kCount; ++i) {
            standard.push_back(static_cast<int>(i));
        }
    });

    double chunked_resize = time_ms([&]() {
        chunked.resize(kCount / 2);
        chunked.resize(kCount);
    });

    double std_resize = time_ms([&]() {
        standard.resize(kCount / 2);
        standard.resize(kCount);
    });

    double chunked_pop = time_ms([&]() {
        for (size_t i = 0; i < kCount / 2; ++i) {
            chunked.pop_back();
        }
    });

    double std_pop = time_ms([&]() {
        for (size_t i = 0; i < kCount / 2; ++i) {
            standard.pop_back();
        }
    });

    double chunked_read = time_ms([&]() { read_sum(chunked); });
    double std_read = time_ms([&]() { read_sum(standard); });

    std::cout << "push_back:  ChunkedVector=" << chunked_push << " ms, std::vector=" << std_push
              << " ms\n";
    std::cout << "resize:     ChunkedVector=" << chunked_resize << " ms, std::vector=" << std_resize
              << " ms\n";
    std::cout << "pop_back:   ChunkedVector=" << chunked_pop << " ms, std::vector=" << std_pop
              << " ms\n";
    std::cout << "read stride:ChunkedVector=" << chunked_read << " ms, std::vector=" << std_read
              << " ms\n";

    return 0;
}
