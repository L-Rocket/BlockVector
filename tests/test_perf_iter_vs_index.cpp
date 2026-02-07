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

    BlockVector<Heavy> block;
    std::vector<Heavy> standard;
    block.resize(count);
    standard.resize(count);

    std::cout << "Benchmark count: " << count << " (Heavy objects)\n";

    double block_index = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            sum += static_cast<size_t>(block[i].payload[0]);
        }
        sink += sum;
    });

    double block_iter = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (auto it = block.begin(); it != block.end(); ++it) {
            sum += static_cast<size_t>((*it).payload[0]);
        }
        sink += sum;
    });

    double std_index = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            sum += static_cast<size_t>(standard[i].payload[0]);
        }
        sink += sum;
    });

    double std_iter = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (auto it = standard.begin(); it != standard.end(); ++it) {
            sum += static_cast<size_t>((*it).payload[0]);
        }
        sink += sum;
    });

    std::cout << "BlockVector index: " << block_index << " ms\n";
    std::cout << "BlockVector iter:  " << block_iter << " ms\n";
    std::cout << "std::vector index: " << std_index << " ms\n";
    std::cout << "std::vector iter:  " << std_iter << " ms\n";

    return 0;
}
