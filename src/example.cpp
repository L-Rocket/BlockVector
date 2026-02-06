#include "example.hpp"

int sum(const std::vector<int>& numbers) {
    int total = 0;
    for (int n : numbers) total += n;
    return total;
}
