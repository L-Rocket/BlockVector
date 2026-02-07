#include "BlockVector.hpp"
#include "BlockVectorDiv.hpp"

#include <chrono>
#include <iostream>

namespace {
constexpr size_t kRounds = 20;

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
    const size_t count = probe.get_Block_size() * 1000;

    BlockVector<Heavy> shift_vec;
    BlockVectorDiv<Heavy> div_vec;
    shift_vec.resize(count);
    div_vec.resize(count);

    std::cout << "Benchmark count: " << count << " (Heavy objects)\n";

    double shift_index = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            sum += static_cast<size_t>(shift_vec[i].payload[0]);
        }
        sink += sum;
    });

    double div_index = avg_ms(kRounds, [&]() {
        size_t sum = 0;
        for (size_t i = 0; i < count; ++i) {
            sum += static_cast<size_t>(div_vec[i].payload[0]);
        }
        sink += sum;
    });

    std::cout << "BlockVector shift index: " << shift_index << " ms\n";
    std::cout << "BlockVector div index:   " << div_index << " ms\n";

    return 0;
}
