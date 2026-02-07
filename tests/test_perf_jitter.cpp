#include "BlockVector.hpp"

#include <chrono>
#include <iostream>
#include <vector>

namespace {
struct StepStats {
    double max_ms = 0.0;
    double total_ms = 0.0;
};

template <typename Func>
StepStats measure_steps(size_t count, Func&& func) {
    StepStats stats;
    for (size_t i = 0; i < count; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        func(i);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        const double ms = elapsed.count();
        stats.total_ms += ms;
        if (ms > stats.max_ms) {
            stats.max_ms = ms;
        }
    }
    return stats;
}
}

int main() {
    BlockVector<int> probe;
    const size_t count = probe.get_Block_size() * 200;

    std::cout << "Benchmark count: " << count << " (int)\n";

    BlockVector<int> block;
    std::vector<int> standard;

    StepStats block_stats = measure_steps(count, [&](size_t i) {
        block.push_back(static_cast<int>(i));
    });

    StepStats std_stats = measure_steps(count, [&](size_t i) {
        standard.push_back(static_cast<int>(i));
    });

    std::cout << "push_back jitter (max step):\n";
    std::cout << "  BlockVector: " << block_stats.max_ms << " ms\n";
    std::cout << "  std::vector: " << std_stats.max_ms << " ms\n";
    std::cout << "push_back total time:\n";
    std::cout << "  BlockVector: " << block_stats.total_ms << " ms\n";
    std::cout << "  std::vector: " << std_stats.total_ms << " ms\n";

    return 0;
}
