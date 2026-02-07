#include "BlockVector.hpp"

#include <iostream>
#include <vector>

int main() {
    BlockVector<int> block;
    std::vector<int> standard;

    const size_t block_size = block.get_Block_size();
    const size_t count = block_size * 4 + 10;

    size_t block_moves = 0;
    size_t standard_moves = 0;

    const int* block_ptr = nullptr;
    const int* standard_ptr = nullptr;

    for (size_t i = 0; i < count; ++i) {
        block.push_back(static_cast<int>(i));
        standard.push_back(static_cast<int>(i));

        const int* new_block_ptr = &block[0];
        const int* new_standard_ptr = &standard[0];

        if (block_ptr && new_block_ptr != block_ptr) {
            ++block_moves;
        }
        if (standard_ptr && new_standard_ptr != standard_ptr) {
            ++standard_moves;
        }

        block_ptr = new_block_ptr;
        standard_ptr = new_standard_ptr;
    }

    std::cout << "Block size: " << block_size << "\n";
    std::cout << "Push count: " << count << "\n";
    std::cout << "Address changes for element[0]:\n";
    std::cout << "  BlockVector: " << block_moves << "\n";
    std::cout << "  std::vector: " << standard_moves << "\n";

    if (block_moves == 0) {
        std::cout << "BlockVector keeps element addresses stable across growth.\n";
    }

    return 0;
}
