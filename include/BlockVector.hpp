#pragma once
#include <cstddef>
#include <vector>

template <typename T>
class ChunkedVector {
private:
    std::vector<std::vector<T>> chunks_;
    size_t block_size_;

    size_t size_;
    size_t capacity_;
public:
    ChunkedVector();
    ChunkedVector(size_t n);
    ChunkedVector(size_t n, const T& value);
    ~ChunkedVector();

    // Element access
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    T& at(size_t index);
    const T& at(size_t index) const;
    T& front();
    const T& front() const;
    T& back();
    const T& back() const;
    
    // Capacity related
    size_t size() const;
    size_t capacity() const;
    bool empty() const;
    void reserve(size_t newCapacity);
    // spacial capacity functions
    size_t get_Block_size() const;
    size_t set_Block_size(size_t new_block_size); // only when empty in v1.0

    // manipulation
    void push_back(const T& value);
    // emplace_back(Args&&...)
    void pop_back();
    void clear();
    void resize(size_t n);
    // bool insert(iterator pos, const T&);
    // bool erase(iterator pos);

    // iterators 
    // TODO
};