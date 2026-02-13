#include <iostream>
#include <vector>

int main() {
    std::vector<int> numbers;
    for (int i = 0; i < 10; ++i) {
        numbers.push_back(i);
        if (i > 1024) {
            std::cout << "Added: " << i << ", size: " << numbers.size() << ", capacity: " << numbers.capacity() << std::endl;
        }

    }
    return 0;
}
