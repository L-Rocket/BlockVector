# BlockVector (ChunkedVector)

BlockVector is a small C++17 library that provides `ChunkedVector<T>`, a vector-like
container backed by fixed-size blocks. It aims for no reallocation during growth and
good spatial locality, while keeping element addresses stable and offering familiar
vector APIs.

## Features (v1.0)

- Blocked storage (`std::vector<std::vector<T>>`) to avoid large contiguous reallocations
- Common element access: `operator[]`, `at()`, `front()`, `back()`
- Capacity control: `size()`, `capacity()`, `reserve()`, `empty()`
- Mutations: `push_back()`, `pop_back()`, `clear()`, `resize()`
- Adjustable block size (currently only when empty)

## Quick Start

Build the examples and tests with CMake:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Usage

```cpp
#include "BlockVector.hpp"

ChunkedVector<int> v;
v.push_back(1);
v.push_back(2);
v.resize(10);
int x = v[0];
```

## Project Layout

- `include/` public headers
- `src/` library and examples
- `tests/` unit tests
- `build/` CMake build output (generated)
- `CMakeLists.txt` build configuration

## Notes

- This project currently focuses on core container behavior. Iterator support and
	insert/erase are planned but not implemented yet.

## Latest Test Run

- Date: 2026-02-07
- Result: 9 passed, 0 failed

## Update CMake List

If you add/remove source files, update the build script:

```bash
python3 ./update_cmake.py
```