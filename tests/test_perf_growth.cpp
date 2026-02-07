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

    double block_push = avg_ms(kRounds, [&]() {
        BlockVector<Heavy> block;
        for (size_t i = 0; i < count; ++i) {
            block.push_back(Heavy(static_cast<int>(i)));
        }
        sink += block.back().payload[0];
    });

    double std_push = avg_ms(kRounds, [&]() {
        std::vector<Heavy> standard;
        for (size_t i = 0; i < count; ++i) {
            standard.push_back(Heavy(static_cast<int>(i)));
        }
        sink += standard.back().payload[0];
    });

    double block_resize = avg_ms(kRounds, [&]() {
        BlockVector<Heavy> block;
        block.resize(count);
        sink += block.back().payload[0];
    });

    double std_resize = avg_ms(kRounds, [&]() {
        std::vector<Heavy> standard;
        standard.resize(count);
        sink += standard.back().payload[0];
    });

    std::cout << "push_back (no reserve): BlockVector=" << block_push
              << " ms, std::vector=" << std_push << " ms\n";
    std::cout << "resize up:              BlockVector=" << block_resize
              << " ms, std::vector=" << std_resize << " ms\n";

    return 0;
}
