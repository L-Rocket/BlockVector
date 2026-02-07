#pragma once
#include <cstddef>
#include <vector>

template <typename T>
class BlockVectorDiv {
private:
    std::vector<std::vector<T>> chunks_;
    size_t size_;
    size_t capacity_;
    size_t block_size_;
public:
    BlockVectorDiv();
    BlockVectorDiv(size_t n);
    BlockVectorDiv(size_t n, const T& value);
    ~BlockVectorDiv();

    class iterator {
    private:
        size_t block_idx_;
        T* cur_;
        T* end_;
        BlockVectorDiv* parent_;
    public:
        iterator(size_t block_idx, T* cur, T* end, BlockVectorDiv* parent)
            : block_idx_(block_idx), cur_(cur), end_(end), parent_(parent) {}

        T& operator*() { return *cur_; }

        iterator& operator++() {
            ++cur_;
            if (cur_ == end_) {
                ++block_idx_;
                if (block_idx_ < parent_->chunks_.size()) {
                    auto& chunk = parent_->chunks_[block_idx_];
                    cur_ = chunk.data();
                    end_ = cur_ + chunk.size();
                } else {
                    cur_ = end_ = nullptr;
                }
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return parent_ != other.parent_ || block_idx_ != other.block_idx_ || cur_ != other.cur_;
        }
    };

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
    size_t set_Block_size(size_t new_block_size);

    // manipulation
    void push_back(const T& value);
    void pop_back();
    void clear();
    void resize(size_t n);

    // iterators
    iterator begin();
    iterator end();
};

#include <stdexcept>

namespace {
constexpr size_t kDefaultBlockSizeDiv = 256;
}

template <typename T>
BlockVectorDiv<T>::BlockVectorDiv()
    : size_(0), capacity_(0), block_size_(kDefaultBlockSizeDiv) {}

template <typename T>
BlockVectorDiv<T>::BlockVectorDiv(size_t n)
    : size_(n), capacity_(0), block_size_(kDefaultBlockSizeDiv) {
    if (n == 0) {
        return;
    }

    while (block_size_ < n) {
        block_size_ <<= 1;
    }
    capacity_ = block_size_;
    chunks_.emplace_back();
    chunks_.back().reserve(block_size_);
    chunks_.back().insert(chunks_.back().end(), n, T());
}

template <typename T>
BlockVectorDiv<T>::BlockVectorDiv(size_t n, const T& value)
    : size_(n), capacity_(0), block_size_(kDefaultBlockSizeDiv) {
    if (n == 0) {
        return;
    }

    while (block_size_ < n) {
        block_size_ <<= 1;
    }
    capacity_ = block_size_;
    chunks_.emplace_back();
    chunks_.back().reserve(block_size_);
    chunks_.back().insert(chunks_.back().end(), n, value);
}

template <typename T>
BlockVectorDiv<T>::~BlockVectorDiv() = default;

template <typename T>
T& BlockVectorDiv<T>::operator[](size_t index) {
    size_t block_index = index / block_size_;
    size_t offset = index % block_size_;
    return chunks_[block_index][offset];
}

template <typename T>
const T& BlockVectorDiv<T>::operator[](size_t index) const {
    size_t block_index = index / block_size_;
    size_t offset = index % block_size_;
    return chunks_[block_index][offset];
}

template <typename T>
T& BlockVectorDiv<T>::at(size_t index) {
    if (index >= size_) {
        throw std::out_of_range("BlockVectorDiv::at");
    }
    return (*this)[index];
}

template <typename T>
const T& BlockVectorDiv<T>::at(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("BlockVectorDiv::at");
    }
    return (*this)[index];
}

template <typename T>
T& BlockVectorDiv<T>::front() {
    return (*this)[0];
}

template <typename T>
const T& BlockVectorDiv<T>::front() const {
    return (*this)[0];
}

template <typename T>
T& BlockVectorDiv<T>::back() {
    return (*this)[size_ - 1];
}

template <typename T>
const T& BlockVectorDiv<T>::back() const {
    return (*this)[size_ - 1];
}

template <typename T>
size_t BlockVectorDiv<T>::size() const {
    return size_;
}

template <typename T>
size_t BlockVectorDiv<T>::capacity() const {
    return capacity_;
}

template <typename T>
bool BlockVectorDiv<T>::empty() const {
    return size_ == 0;
}

template <typename T>
void BlockVectorDiv<T>::reserve(size_t newCapacity) {
    if (newCapacity <= capacity_) {
        return;
    }
    if (block_size_ == 0) {
        block_size_ = kDefaultBlockSizeDiv;
    }

    size_t required_blocks = (newCapacity + block_size_ - 1) / block_size_;
    if (required_blocks <= chunks_.size()) {
        capacity_ = required_blocks * block_size_;
        return;
    }

    size_t to_add = required_blocks - chunks_.size();
    for (size_t i = 0; i < to_add; ++i) {
        chunks_.emplace_back();
        chunks_.back().reserve(block_size_);
    }
    capacity_ = required_blocks * block_size_;
}

template <typename T>
size_t BlockVectorDiv<T>::get_Block_size() const {
    return block_size_;
}

template <typename T>
size_t BlockVectorDiv<T>::set_Block_size(size_t new_block_size) {
    if (size_ != 0 || new_block_size == 0) {
        return block_size_;
    }

    size_t rounded = 1;
    while (rounded < new_block_size) {
        rounded <<= 1;
    }
    block_size_ = rounded;
    chunks_.clear();
    capacity_ = 0;
    return block_size_;
}

template <typename T>
void BlockVectorDiv<T>::push_back(const T& value) {
    if (size_ == capacity_) {
        chunks_.emplace_back();
        chunks_.back().reserve(block_size_);
        capacity_ += block_size_;
    }
    chunks_.back().push_back(value);
    ++size_;
}

template <typename T>
void BlockVectorDiv<T>::pop_back() {
    if (size_ == 0) {
        return;
    }
    chunks_.back().pop_back();
    --size_;
    if (chunks_.back().empty() && chunks_.size() > 1) {
        chunks_.pop_back();
        capacity_ -= block_size_;
    }
}

template <typename T>
void BlockVectorDiv<T>::clear() {
    for (auto& chunk : chunks_) {
        chunk.clear();
    }
    size_ = 0;
}

template <typename T>
void BlockVectorDiv<T>::resize(size_t n) {
    if (n < size_) {
        while (size_ > n) {
            pop_back();
        }
        return;
    }

    if (n > size_) {
        reserve(n);
        size_t remaining = n - size_;
        while (remaining > 0) {
            size_t room = block_size_ - chunks_.back().size();
            size_t count = std::min(remaining, room);
            chunks_.back().insert(chunks_.back().end(), count, T{});
            size_ += count;
            remaining -= count;
            if (remaining > 0) {
                chunks_.emplace_back();
                chunks_.back().reserve(block_size_);
            }
        }
    }
}

template <typename T>
typename BlockVectorDiv<T>::iterator BlockVectorDiv<T>::begin() {
    if (size_ == 0) {
        return end();
    }

    size_t block_idx = 0;
    while (block_idx < chunks_.size() && chunks_[block_idx].empty()) {
        ++block_idx;
    }

    if (block_idx >= chunks_.size()) {
        return end();
    }

    auto& chunk = chunks_[block_idx];
    return iterator(block_idx, chunk.data(), chunk.data() + chunk.size(), this);
}

template <typename T>
typename BlockVectorDiv<T>::iterator BlockVectorDiv<T>::end() {
    return iterator(chunks_.size(), nullptr, nullptr, this);
}
