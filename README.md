# BlockVector

[![C++ CI](https://github.com/L-Rocket/BlockVector/actions/workflows/ci.yml/badge.svg)](https://github.com/L-Rocket/BlockVector/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/L-Rocket/BlockVector/graph/badge.svg?token=YOUR_TOKEN)](https://codecov.io/gh/L-Rocket/BlockVector)

**BlockVector** 是一个高性能、仅头文件（Header-only）的 C++17 容器，旨在解决 `std::vector` 在特定场景下的局限性。

---

### 🚀 为什么选择 BlockVector？

在 C++ 开发中，`std::vector` 是默认的首选容器，但它在以下场景中存在痛点：
1.  **指针失效 (Pointer Invalidation)**：当 `std::vector` 扩容时，它会重新分配一块更大的连续内存并将所有元素移动过去。这会导致之前指向容器元素的任何指针、引用或迭代器全部失效。
2.  **昂贵的重定位开销**：对于**大对象**或构造代价高昂的对象，扩容时的批量拷贝/移动操作会带来显著的性能抖动。
3.  **内存碎片与峰值**：在处理海量数据时，`std::vector` 的倍增式扩容可能导致瞬时的内存需求翻倍，容易触发 OOM 或产生内存碎片。

**BlockVector 通过“分块存储”完美解决了这些问题：**
-   **绝对的指针稳定性**：一旦元素被存入，其内存地址在容器的整个生命周期内保持不变。即使容器持续增长，你的指针也永远不会失效。
-   **零拷贝扩容**：增长时只需分配新的固定大小 Block，无需搬运旧数据。
-   **平滑的内存轨迹**：增量式分配，内存占用预测性更强。

---

## 主要特性

- **仅头文件**: 无需编译，直接包含即可使用。
- **指针稳定性**: 保证在 `push_back` 过程中，现有元素的引用和指针始终有效。
- **O(1) 随机访问**: 依然支持快速的 `operator[]` 索引访问。
- **标准兼容**: 完整的 `RandomAccessIterator` 支持，可直接用于 `std::sort` 等算法。
- **现代 C++ 接口**: 支持初始化列表 `{1, 2, 3}` 和原位构造 `emplace_back`。
- **STL 符合性**: 提供完整的 Type Traits (`value_type`, `size_type` 等)，完美适配泛型库。

## 安装方式

### 方法 1: CMake FetchContent (推荐)

在你的 `CMakeLists.txt` 中添加：

```cmake
include(FetchContent)
FetchContent_Declare(
    BlockVector
    GIT_REPOSITORY https://github.com/L-Rocket/BlockVector.git
    GIT_TAG main 
)
FetchContent_MakeAvailable(BlockVector)

# 链接到你的目标
target_link_libraries(YourApp PRIVATE BlockVector)
```

### 方法 2: 直接下载 (Release)

从 GitHub Releases 下载最新的稳定版。

**使用 curl:**
```bash
curl -L -O https://github.com/L-Rocket/BlockVector/releases/latest/download/BlockVector.hpp
```

**使用 wget:**
```bash
wget https://github.com/L-Rocket/BlockVector/releases/latest/download/BlockVector.hpp
```

## 快速上手

```cpp
#include <iostream>
#include <algorithm>
#include "BlockVector.hpp"

int main() {
    // 1. 初始化
    BlockVector<int> bv = {10, 5, 20};
    
    // 2. 指针稳定性演示
    int& ref = bv[0];
    for(int i=0; i<10000; ++i) bv.push_back(i); 
    // 即便插入了一万个元素，ref 依然指向原来的 10，不会失效！
    std::cout << "Original element: " << ref << std::endl;

    // 3. 配合 STL 算法
    std::sort(bv.begin(), bv.end());
    
    return 0;
}
```

## 适用场景

- **对象池 / 内存池实现**：需要稳定管理大量长生命周期对象。
- **大型 3D 模型/点云数据**：对象体积大，搬迁成本极高。
- **高频日志/事件流缓冲区**：持续追加数据且不希望出现扩容导致的卡顿。

## 开源协议

MIT License
