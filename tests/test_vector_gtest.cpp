#include <gtest/gtest.h>
#include "BlockVector.hpp"
#include <algorithm>
#include <numeric>

// Basic Push/Pop Test
TEST(BlockVectorTest, PushBackAndAccess) {
    BlockVector<int> bv;
    bv.push_back(10);
    bv.push_back(20);

    EXPECT_EQ(bv.size(), 2);
    EXPECT_EQ(bv[0], 10);
    EXPECT_EQ(bv[1], 20);
    EXPECT_EQ(bv.front(), 10);
    EXPECT_EQ(bv.back(), 20);
}

// Iterator Test (The feature we just fixed)
TEST(BlockVectorTest, IteratorTraversal) {
    BlockVector<int> bv;
    for (int i = 0; i < 100; ++i) {
        bv.push_back(i);
    }

    int count = 0;
    for (auto it = bv.begin(); it != bv.end(); ++it) {
        EXPECT_EQ(*it, count);
        count++;
    }
    EXPECT_EQ(count, 100);
}

// Algorithm Test (std::sort)
TEST(BlockVectorTest, StdSort) {
    BlockVector<int> bv;
    bv.push_back(5);
    bv.push_back(1);
    bv.push_back(3);
    bv.push_back(2);
    bv.push_back(4);

    std::sort(bv.begin(), bv.end());

    EXPECT_EQ(bv[0], 1);
    EXPECT_EQ(bv[1], 2);
    EXPECT_EQ(bv[2], 3);
    EXPECT_EQ(bv[3], 4);
    EXPECT_EQ(bv[4], 5);
}

// Reverse Iterator Test
TEST(BlockVectorTest, ReverseIterator) {
    BlockVector<int> bv;
    bv.push_back(1);
    bv.push_back(2);
    bv.push_back(3);

    auto it = bv.rbegin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, bv.rend());
}

// Capacity Test
TEST(BlockVectorTest, CapacityGrowth) {
    BlockVector<int> bv;
    size_t initial_cap = bv.capacity();
    
    // Fill beyond initial capacity to trigger resize
    size_t n = 1000;
    for(size_t i=0; i<n; ++i) {
        bv.push_back(i);
    }
    
    EXPECT_EQ(bv.size(), n);
    EXPECT_GE(bv.capacity(), n);
}

struct ComplexObj {
    int x;
    double y;
    ComplexObj(int a, double b) : x(a), y(b) {}
};

TEST(BlockVectorTest, EmplaceBack) {
    BlockVector<ComplexObj> bv;
    bv.emplace_back(1, 2.5);
    bv.emplace_back(2, 3.5);
    
    EXPECT_EQ(bv.size(), 2);
    EXPECT_EQ(bv[0].x, 1);
    EXPECT_EQ(bv[0].y, 2.5);
    EXPECT_EQ(bv[1].x, 2);
}
