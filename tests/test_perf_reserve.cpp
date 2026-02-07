#include "BlockVector.hpp"

#include <chrono>
#include <iostream>
#include <vector>

namespace {
constexpr size_t kRounds = 3;

struct Heavy {
    int payload[64];

    explicit Heavy(int seed = 0) {
        for (size_t i = 0; i < 64; ++i) {
            payload[i] = seed + static_cast<int>(i);
        }
    }

    Heavy(const Heavy& other) {
        for (size_t i = 0; i < 64; ++i) {
            payload[i] = other.payload[i];
        }
    }

    Heavy& operator=(const Heavy& other) {
        if (this != &other) {
            for (size_t i = 0; i < 64; ++i) {
                payload[i] = other.payload[i];
            }
        }
        return *this;
    }
};

template <typename Func>
double time_ms(Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    return elapsed.count();
}

template <typename Func>
double avg_ms(size_t rounds, Func&& func) {
    double total = 0.0;
    for (size_t i = 0; i < rounds; ++i) {
        total += time_ms(func);
    }
    return total / static_cast<double>(rounds);
}

volatile size_t sink = 0;
}

int main() {
    BlockVector<int> probe;
    const size_t count = probe.get_Block_size() * 200;

    std::cout << "Benchmark count: " << count << " (Heavy objects)\n";

    double block_no_reserve = avg_ms(kRounds, [&]() {
        BlockVector<Heavy> block;
        for (size_t i = 0; i < count; ++i) {
            block.push_back(Heavy(static_cast<int>(i)));
        }
        sink += block.back().payload[0];
    });

    double std_no_reserve = avg_ms(kRounds, [&]() {
        std::vector<Heavy> standard;
        for (size_t i = 0; i < count; ++i) {
            standard.push_back(Heavy(static_cast<int>(i)));
        }
        sink += standard.back().payload[0];
    });

    double std_with_reserve = avg_ms(kRounds, [&]() {
        std::vector<Heavy> standard;
        standard.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            standard.push_back(Heavy(static_cast<int>(i)));
        }
        sink += standard.back().payload[0];
    });

    std::cout << "push_back (BlockVector, no reserve): " << block_no_reserve << " ms\n";
    std::cout << "push_back (std::vector, no reserve): " << std_no_reserve << " ms\n";
    std::cout << "push_back (std::vector, reserve):    " << std_with_reserve << " ms\n";

    return 0;
}
