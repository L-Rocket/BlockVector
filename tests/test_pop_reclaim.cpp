#include "BlockVector.hpp"

#include <iostream>
#include <vector>

int main() {
    BlockVector<int> probe;
    const size_t count = probe.get_Block_size() * 200;

    BlockVector<int> block;
    std::vector<int> standard;

    for (size_t i = 0; i < count; ++i) {
        block.push_back(static_cast<int>(i));
        standard.push_back(static_cast<int>(i));
    }

    const size_t pop_count = count - probe.get_Block_size() / 2;
    for (size_t i = 0; i < pop_count; ++i) {
        block.pop_back();
        standard.pop_back();
    }

    std::cout << "Initial count: " << count << "\n";
    std::cout << "After pop_back: " << block.size() << "\n";
    std::cout << "BlockVector capacity: " << block.capacity() << "\n";
    std::cout << "std::vector capacity: " << standard.capacity() << "\n";

    return 0;
}
