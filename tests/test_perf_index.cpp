#include "BlockVector.hpp"

#include <chrono>
#include <iostream>
#include <random>
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

    std::vector<size_t> indices;
    indices.reserve(count);

    std::mt19937 rng(12345);
    std::uniform_int_distribution<size_t> dist(0, count - 1);
    for (size_t i = 0; i < count; ++i) {
        indices.push_back(dist(rng));
    }

    BlockVector<Heavy> block;
    std::vector<Heavy> standard;
    block.resize(count);
    standard.resize(count);

    std::cout << "Benchmark count: " << count << " (Heavy objects)\n";

    double block_seq = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            sum += static_cast<size_t>(block[i].payload[0]);
        }
        sink += sum;
    });

    double std_seq = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            sum += static_cast<size_t>(standard[i].payload[0]);
        }
        sink += sum;
    });

    double block_rand = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            const size_t idx = indices[i];
            sum += static_cast<size_t>(block[idx].payload[0]);
        }
        sink += sum;
    });

    double std_rand = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            const size_t idx = indices[i];
            sum += static_cast<size_t>(standard[idx].payload[0]);
        }
        sink += sum;
    });

    std::cout << "index seq:  BlockVector=" << block_seq << " ms, std::vector=" << std_seq
              << " ms\n";
    std::cout << "index rand: BlockVector=" << block_rand << " ms, std::vector=" << std_rand
              << " ms\n";

    return 0;
}
