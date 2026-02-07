#include <gtest/gtest.h>
#include "BlockVector.hpp"
#include <type_traits>

TEST(BlockVectorTraits, StandardAliases) {
    using BV = BlockVector<int>;
    
    // Check basic types
    static_assert(std::is_same<BV::value_type, int>::value, "value_type must be int");
    static_assert(std::is_same<BV::size_type, size_t>::value, "size_type must be size_t");
    static_assert(std::is_same<BV::difference_type, std::ptrdiff_t>::value, "difference_type mismatch");
    
    // Check reference/pointer types
    static_assert(std::is_same<BV::reference, int&>::value, "reference mismatch");
    static_assert(std::is_same<BV::const_reference, const int&>::value, "const_reference mismatch");
    static_assert(std::is_same<BV::pointer, int*>::value, "pointer mismatch");
    static_assert(std::is_same<BV::const_pointer, const int*>::value, "const_pointer mismatch");

    SUCCEED(); // Static asserts passed
}

TEST(BlockVectorTraits, IteratorTypes) {
    using BV = BlockVector<int>;
    
    static_assert(!std::is_const<std::remove_reference<BV::iterator::reference>::type>::value, "iterator should be non-const");
    static_assert(std::is_const<std::remove_reference<BV::const_iterator::reference>::type>::value, "const_iterator should be const");
    
    SUCCEED();
}
