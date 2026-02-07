#include "BlockVector.hpp"

#include <chrono>
#include <iostream>
#include <vector>

namespace {
constexpr size_t kRounds = 3;

struct Small {
    int value;
    explicit Small(int v = 0) : value(v) {}
};

struct Large {
    int payload[64];

    explicit Large(int seed = 0) {
        for (size_t i = 0; i < 64; ++i) {
            payload[i] = seed + static_cast<int>(i);
        }
    }

    Large(const Large& other) {
        for (size_t i = 0; i < 64; ++i) {
            payload[i] = other.payload[i];
        }
    }

    Large& operator=(const Large& other) {
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

    std::cout << "Benchmark count: " << count << "\n";

    double block_small = avg_ms(kRounds, [&]() {
        BlockVector<Small> block;
        for (size_t i = 0; i < count; ++i) {
            block.push_back(Small(static_cast<int>(i)));
        }
        sink += static_cast<size_t>(block.back().value);
    });

    double std_small = avg_ms(kRounds, [&]() {
        std::vector<Small> standard;
        for (size_t i = 0; i < count; ++i) {
            standard.push_back(Small(static_cast<int>(i)));
        }
        sink += static_cast<size_t>(standard.back().value);
    });

    double block_large = avg_ms(kRounds, [&]() {
        BlockVector<Large> block;
        for (size_t i = 0; i < count; ++i) {
            block.push_back(Large(static_cast<int>(i)));
        }
        sink += static_cast<size_t>(block.back().payload[0]);
    });

    double std_large = avg_ms(kRounds, [&]() {
        std::vector<Large> standard;
        for (size_t i = 0; i < count; ++i) {
            standard.push_back(Large(static_cast<int>(i)));
        }
        sink += static_cast<size_t>(standard.back().payload[0]);
    });

    std::cout << "Small push_back: BlockVector=" << block_small
              << " ms, std::vector=" << std_small << " ms\n";
    std::cout << "Large push_back: BlockVector=" << block_large
              << " ms, std::vector=" << std_large << " ms\n";

    return 0;
}
