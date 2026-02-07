# BlockVector

[![C++ CI](https://github.com/L-Rocket/BlockVector/actions/workflows/ci.yml/badge.svg)](https://github.com/L-Rocket/BlockVector/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/L-Rocket/BlockVector/graph/badge.svg?token=YOUR_TOKEN)](https://codecov.io/gh/L-Rocket/BlockVector)

[**中文文档 (Chinese)**](README_zh.md)

**BlockVector** is a high-performance, header-only C++17 container designed to solve specific limitations of `std::vector`.

---

### 🚀 Why BlockVector?

While `std::vector` is the default choice for dynamic arrays, it has significant drawbacks in certain scenarios:

1.  **Pointer Invalidation**: When `std::vector` grows, it reallocates memory and moves all elements to a new location. This invalidates all existing pointers, references, and iterators to the elements.
2.  **Expensive Reallocation**: For **large objects** or objects with expensive constructors, copying or moving them during expansion causes severe performance spikes.
3.  **Memory Peaks**: The doubling expansion strategy of `std::vector` can cause temporary memory usage to double, potentially leading to OOM (Out of Memory) errors with massive datasets.

**BlockVector solves these problems with "Chunked Storage":**
-   **Pointer Stability**: Guaranteed pointer/reference validity for the entire lifetime of the container. Pointers to elements *never* break when you `push_back`.
-   **Zero-Copy Growth**: Expansion only involves allocating a new fixed-size block. No old elements are ever moved or copied.
-   **Predictable Memory**: Incremental allocation leads to a smoother memory usage curve.

---

## Features

- **Header-only**: Zero-dependency integration.
- **Pointer Stability**: References to elements remain valid forever (until the container is destroyed).
- **O(1) Random Access**: Fast `operator[]` access just like `std::vector`.
- **Standard Compliant**: Full `RandomAccessIterator` support, compatible with `std::sort`, `std::lower_bound`.
- **Modern C++**: Supports Initializer Lists (`{1, 2, 3}`) and in-place construction via `emplace_back`.
- **Type Traits**: Provides standard type aliases (`value_type`, `size_type`, etc.) for seamless compatibility with template metaprogramming libraries.

## Installation

### Method 1: CMake FetchContent (Recommended)

Add this to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
    BlockVector
    GIT_REPOSITORY https://github.com/L-Rocket/BlockVector.git
    GIT_TAG main 
)
FetchContent_MakeAvailable(BlockVector)

# Link to your target
target_link_libraries(YourApp PRIVATE BlockVector)
```

### Method 2: Copy Header (Simple)

Download the latest stable version directly from GitHub Releases.

**Using curl:**
```bash
curl -L -O https://github.com/L-Rocket/BlockVector/releases/latest/download/BlockVector.hpp
```

**Using wget:**
```bash
wget https://github.com/L-Rocket/BlockVector/releases/latest/download/BlockVector.hpp
```

## Quick Start

```cpp
#include <iostream>
#include <algorithm>
#include "BlockVector.hpp"

struct Point { int x, y; Point(int a, int b): x(a), y(b){} };

int main() {
    // 1. Initialization
    BlockVector<int> bv = {10, 5, 20};
    
    // 2. Pointer Stability Demo
    int& ref = bv[0]; // Reference to the first element (10)
    
    // Add thousands of elements to trigger multiple expansions
    for(int i=0; i<10000; ++i) {
        bv.push_back(i); 
    }
    
    // In std::vector, 'ref' would be a dangling reference here.
    // In BlockVector, it is still valid!
    std::cout << "Original element is still: " << ref << std::endl;

    // 3. Use with Standard Algorithms
    std::sort(bv.begin(), bv.end());
    
    return 0;
}
```

## Use Cases

- **Object Pools**: Managing objects that must stay at fixed addresses.
- **Large 3D Models / Point Clouds**: Storing massive objects where moving them is too expensive.
- **Event/Log Buffers**: High-frequency appending where latency spikes from reallocation are unacceptable.

## License

MIT License
