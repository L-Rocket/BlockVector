#include "example.hpp"
#include <cassert>
#include <vector>

int main() {
    std::vector<int> nums = {1, 2, 3};
    assert(sum(nums) == 6);
    return 0;
}
