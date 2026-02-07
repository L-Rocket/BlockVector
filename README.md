# BlockVector

[![C++ CI](https://github.com/L-Rocket/BlockVector/actions/workflows/ci.yml/badge.svg)](https://github.com/L-Rocket/BlockVector/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/L-Rocket/BlockVector/graph/badge.svg?token=YOUR_TOKEN)](https://codecov.io/gh/L-Rocket/BlockVector)

BlockVector is a high-performance, header-only C++17 library that provides `BlockVector<T>`, a container backed by fixed-size blocks. It guarantees **pointer stability** (no reallocation of existing elements during growth) and offers familiar `std::vector`-like APIs.

## Features

- **Header-only**: Easy integration, no linking required.
- **Pointer Stability**: Pointers/References to elements remain valid after `push_back`.
- **Random Access**: Fast `operator[]` access (O(1)).
- **Complete Iterators**: Fully compliant `RandomAccessIterator` (compatible with `std::sort`, `std::lower_bound`).
- **Modern C++**: Support for **Initializer Lists** (`{1, 2, 3}`) and **`emplace_back`**.
- **Standard API**: Drop-in replacement for `std::vector` in most cases (`push_back`, `pop_back`, `resize`, `reserve`).

## Integration

### Method 1: CMake FetchContent (Recommended)

Add this to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
    BlockVector
    GIT_REPOSITORY https://github.com/L-Rocket/BlockVector.git
    GIT_TAG main  # Or a specific tag like v1.0.0
)
FetchContent_MakeAvailable(BlockVector)

# Link to your target
add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE BlockVector)
```

### Method 2: Copy Header (Simple)

You can download the latest stable version directly from GitHub Releases.

**Using curl:**
```bash
# Note: -L is required to follow redirects
curl -L -O https://github.com/L-Rocket/BlockVector/releases/latest/download/BlockVector.hpp
```

**Using wget:**
```bash
wget https://github.com/L-Rocket/BlockVector/releases/latest/download/BlockVector.hpp
```

## Usage

```cpp
#include <iostream>
#include <algorithm>
#include "BlockVector.hpp"

struct Point { int x, y; Point(int a, int b): x(a), y(b){} };

int main() {
    // 1. Initializer List
    BlockVector<int> bv = {10, 5, 20, 15};
    
    // 2. Emplace Back (No copy)
    BlockVector<Point> points;
    points.emplace_back(1, 2);

    // 3. Random Access
    std::cout << "Element at 1: " << bv[1] << std::endl; // 5

    // 4. Use with Standard Algorithms
    std::sort(bv.begin(), bv.end()); // 5, 10, 15, 20

    // 5. Iterate
    for (const auto& val : bv) {
        std::cout << val << " ";
    }
    
    return 0;
}
```

## Building Tests

If you are developing `BlockVector` itself:

```bash
mkdir build && cd build
cmake ..
make
ctest
```

## License

MIT License