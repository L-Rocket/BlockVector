#pragma once
#include <cstddef>
#include <vector>
#include <iterator>
#include <type_traits>
#include <stdexcept>

// Forward declarations
template <typename T>
class BlockVector;

template <typename T, bool IsConst>
class BlockVectorIterator;

template <typename T>
class BlockVector {
private:
    std::vector<std::vector<T>> chunks_;
    size_t size_;
    size_t capacity_;
    size_t block_size_;
    size_t block_shift_;
    size_t block_mask_;

    void update_block_shift();
public:
    using iterator = BlockVectorIterator<T, false>;
    using const_iterator = BlockVectorIterator<T, true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // Allow iterator to access private members
    friend class BlockVectorIterator<T, false>;
    friend class BlockVectorIterator<T, true>;

    BlockVector();
    BlockVector(size_t n);
    BlockVector(size_t n, const T& value);
    ~BlockVector();

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
    
    // iterators 
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;
};

// Iterator Implementation
template <typename T, bool IsConst>
class BlockVectorIterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = typename std::conditional<IsConst, const T*, T*>::type;
    using reference         = typename std::conditional<IsConst, const T&, T&>::type;
    using parent_ptr        = typename std::conditional<IsConst, const BlockVector<T>*, BlockVector<T>*>::type;

private:
    size_t block_idx_;
    T* cur_;
    T* end_;
    parent_ptr parent_;

    friend class BlockVector<T>;
    friend class BlockVectorIterator<T, !IsConst>;

public:
    BlockVectorIterator() : block_idx_(0), cur_(nullptr), end_(nullptr), parent_(nullptr) {}

    BlockVectorIterator(size_t block_idx, T* cur, T* end, parent_ptr parent)
        : block_idx_(block_idx), cur_(const_cast<T*>(cur)), end_(const_cast<T*>(end)), parent_(parent) {}

    template <bool WasConst, typename = typename std::enable_if<IsConst && !WasConst>::type>
    BlockVectorIterator(const BlockVectorIterator<T, WasConst>& other)
        : block_idx_(other.block_idx_), cur_(other.cur_), end_(other.end_), parent_(other.parent_) {}

    reference operator*() const { return *cur_; }
    pointer operator->() const { return cur_; }

    BlockVectorIterator& operator++() {
        ++cur_;
        if (cur_ == end_) {
            ++block_idx_;
            if (block_idx_ < parent_->chunks_.size()) {
                auto& chunk = parent_->chunks_[block_idx_];
                if (!chunk.empty()) {
                    cur_ = const_cast<T*>(chunk.data());
                    end_ = cur_ + chunk.size();
                } else {
                    cur_ = end_ = nullptr;
                }
            } else {
                cur_ = end_ = nullptr;
            }
        }
        return *this;
    }

    BlockVectorIterator operator++(int) {
        BlockVectorIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    BlockVectorIterator& operator--() {
        if (cur_ == nullptr) {
            block_idx_ = parent_->chunks_.size();
            while (block_idx_ > 0) {
                --block_idx_;
                auto& chunk = parent_->chunks_[block_idx_];
                if (!chunk.empty()) {
                    cur_ = const_cast<T*>(chunk.data() + chunk.size() - 1);
                    end_ = const_cast<T*>(chunk.data() + chunk.size());
                    return *this;
                }
            }
            return *this;
        }
        auto& chunk = parent_->chunks_[block_idx_];
        if (cur_ == chunk.data()) {
            while (block_idx_ > 0) {
                --block_idx_;
                auto& prev_chunk = parent_->chunks_[block_idx_];
                if (!prev_chunk.empty()) {
                    cur_ = const_cast<T*>(prev_chunk.data() + prev_chunk.size() - 1);
                    end_ = const_cast<T*>(prev_chunk.data() + prev_chunk.size());
                    return *this;
                }
            }
        } else {
            --cur_;
        }
        return *this;
    }

    BlockVectorIterator operator--(int) {
        BlockVectorIterator tmp = *this;
        --(*this);
        return tmp;
    }

    BlockVectorIterator& operator+=(difference_type n) {
        if (n == 0) return *this;
        if (n < 0) return *this -= (-n);
        size_t global_idx;
        if (cur_ == nullptr) global_idx = parent_->size();
        else global_idx = (block_idx_ << parent_->block_shift_) + (cur_ - const_cast<T*>(parent_->chunks_[block_idx_].data()));
        
        global_idx += n;
        if (global_idx >= parent_->size()) {
            block_idx_ = parent_->chunks_.size();
            cur_ = end_ = nullptr;
            return *this;
        }
        block_idx_ = global_idx >> parent_->block_shift_;
        size_t offset = global_idx & parent_->block_mask_;
        auto& chunk = parent_->chunks_[block_idx_];
        cur_ = const_cast<T*>(chunk.data() + offset);
        end_ = const_cast<T*>(chunk.data() + chunk.size());
        return *this;
    }

    BlockVectorIterator& operator-=(difference_type n) {
        if (n == 0) return *this;
        if (n < 0) return *this += (-n);
        size_t global_idx;
        if (cur_ == nullptr) global_idx = parent_->size();
        else global_idx = (block_idx_ << parent_->block_shift_) + (cur_ - const_cast<T*>(parent_->chunks_[block_idx_].data()));
        
        if (static_cast<size_t>(n) > global_idx) global_idx = 0;
        else global_idx -= n;
        
        block_idx_ = global_idx >> parent_->block_shift_;
        size_t offset = global_idx & parent_->block_mask_;
        auto& chunk = parent_->chunks_[block_idx_];
        cur_ = const_cast<T*>(chunk.data() + offset);
        end_ = const_cast<T*>(chunk.data() + chunk.size());
        return *this;
    }

    BlockVectorIterator operator+(difference_type n) const { return BlockVectorIterator(*this) += n; }
    BlockVectorIterator operator-(difference_type n) const { return BlockVectorIterator(*this) -= n; }
    
    difference_type operator-(const BlockVectorIterator& other) const {
        size_t my_global = (cur_ == nullptr) ? parent_->size() : (block_idx_ << parent_->block_shift_) + (cur_ - const_cast<T*>(parent_->chunks_[block_idx_].data()));
        size_t other_global = (other.cur_ == nullptr) ? other.parent_->size() : (other.block_idx_ << other.parent_->block_shift_) + (other.cur_ - const_cast<T*>(other.parent_->chunks_[other.block_idx_].data()));
        return static_cast<difference_type>(my_global) - static_cast<difference_type>(other_global);
    }

    reference operator[](difference_type n) const { return *(*this + n); }

    bool operator==(const BlockVectorIterator& other) const { return block_idx_ == other.block_idx_ && cur_ == other.cur_; }
    bool operator!=(const BlockVectorIterator& other) const { return !(*this == other); }
    bool operator<(const BlockVectorIterator& other) const {
            bool this_end = (cur_ == nullptr);
            bool other_end = (other.cur_ == nullptr);
            if (this_end) return false;
            if (other_end) return true;
            if (block_idx_ != other.block_idx_) return block_idx_ < other.block_idx_;
            return cur_ < other.cur_;
    }
    bool operator>(const BlockVectorIterator& other) const { return other < *this; }
    bool operator<=(const BlockVectorIterator& other) const { return !(*this > other); }
    bool operator>=(const BlockVectorIterator& other) const { return !(*this < other); }

    friend BlockVectorIterator operator+(difference_type n, const BlockVectorIterator& it) { return it + n; }
};

// BlockVector Definitions

namespace {
constexpr size_t kDefaultBlockSize = 256;
const size_t kDefaultBlockShift = 8;
}

template <typename T>
BlockVector<T>::BlockVector()
    : size_(0), capacity_(0), block_size_(kDefaultBlockSize), block_shift_(kDefaultBlockShift), block_mask_(kDefaultBlockSize - 1) {
}

template <typename T>
BlockVector<T>::BlockVector(size_t n)
    : size_(n), capacity_(0), block_size_(kDefaultBlockSize), block_shift_(kDefaultBlockShift), block_mask_(kDefaultBlockSize - 1) {
    if (n == 0) {
        return;
    }

    while (block_size_ < n) {
        block_size_ <<= 1;
    }
    update_block_shift();
    capacity_ = block_size_;
    chunks_.emplace_back();
    chunks_.back().reserve(block_size_);
    chunks_.back().insert(chunks_.back().end(), n, T());
}

template <typename T>
BlockVector<T>::BlockVector(size_t n, const T& value)
    : size_(n), capacity_(0), block_size_(kDefaultBlockSize), block_shift_(kDefaultBlockShift), block_mask_(kDefaultBlockSize - 1) {
    if (n == 0) {
        return;
    }

    while (block_size_ < n) {
        block_size_ <<= 1;
    }
    update_block_shift();
    capacity_ = block_size_;
    chunks_.emplace_back();
    chunks_.back().reserve(block_size_);
    chunks_.back().insert(chunks_.back().end(), n, value);
}

template <typename T>
BlockVector<T>::~BlockVector() = default;

template <typename T>
T& BlockVector<T>::operator[](size_t index) {
    size_t block_index = index >> block_shift_;
    // size_t offset = index % block_size_;
    size_t offset = index & block_mask_;
    return chunks_[block_index][offset];
}

template <typename T>
const T& BlockVector<T>::operator[](size_t index) const {
    size_t block_index = index >> block_shift_;
    // size_t offset = index % block_size_;
    size_t offset = index & block_mask_;
    return chunks_[block_index][offset];
}

template <typename T>
T& BlockVector<T>::at(size_t index) {
    if (index >= size_) {
        throw std::out_of_range("BlockVector::at");
    }
    return (*this)[index];
}

template <typename T>
const T& BlockVector<T>::at(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("BlockVector::at");
    }
    return (*this)[index];
}

template <typename T>
T& BlockVector<T>::front() {
    return (*this)[0];
}

template <typename T>
const T& BlockVector<T>::front() const {
    return (*this)[0];
}

template <typename T>
T& BlockVector<T>::back() {
    return (*this)[size_ - 1];
}

template <typename T>
const T& BlockVector<T>::back() const {
    return (*this)[size_ - 1];
}

template <typename T>
size_t BlockVector<T>::size() const {
    return size_;
}

template <typename T>
size_t BlockVector<T>::capacity() const {
    return capacity_;
}

template <typename T>
bool BlockVector<T>::empty() const {
    return size_ == 0;
}

template <typename T>
void BlockVector<T>::reserve(size_t newCapacity) {
    if (newCapacity <= capacity_) {
        return;
    }
    if (block_size_ == 0) {
        block_size_ = kDefaultBlockSize;
        block_shift_ = kDefaultBlockShift;
        block_mask_ = block_size_ - 1;
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
size_t BlockVector<T>::get_Block_size() const {
    return block_size_;
}

template <typename T>
size_t BlockVector<T>::set_Block_size(size_t new_block_size) {
    if (size_ != 0 || new_block_size == 0) {
        return block_size_;
    }

    size_t rounded = 1;
    while (rounded < new_block_size) {
        rounded <<= 1;
    }
    block_size_ = rounded;
    update_block_shift();
    chunks_.clear();
    capacity_ = 0;
    return block_size_;
}

template <typename T>
void BlockVector<T>::update_block_shift() {
    block_shift_ = 0;
    size_t value = block_size_;
    while (value > 1) {
        value >>= 1;
        ++block_shift_;
    }
    block_mask_ = block_size_ - 1;
}

template <typename T>
void BlockVector<T>::push_back(const T& value) {
    if (size_ == capacity_) {
        chunks_.emplace_back();
        chunks_.back().reserve(block_size_);
        capacity_ += block_size_;
    }
    chunks_.back().push_back(value);
    ++size_;
}

template <typename T>
void BlockVector<T>::pop_back() {
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
void BlockVector<T>::clear() {
    for (auto& chunk : chunks_) {
        chunk.clear();
    }
    size_ = 0;
}

template <typename T>
void BlockVector<T>::resize(size_t n) {
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
typename BlockVector<T>::iterator BlockVector<T>::begin() {
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
typename BlockVector<T>::iterator BlockVector<T>::end() {
    return iterator(chunks_.size(), nullptr, nullptr, this);
}

template <typename T>
typename BlockVector<T>::const_iterator BlockVector<T>::begin() const {
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

    const auto& chunk = chunks_[block_idx];
    return const_iterator(block_idx, const_cast<T*>(chunk.data()), const_cast<T*>(chunk.data() + chunk.size()), this);
}

template <typename T>
typename BlockVector<T>::const_iterator BlockVector<T>::cbegin() const {
    return begin();
}

template <typename T>
typename BlockVector<T>::const_iterator BlockVector<T>::end() const {
    return const_iterator(chunks_.size(), nullptr, nullptr, this);
}

template <typename T>
typename BlockVector<T>::const_iterator BlockVector<T>::cend() const {
    return end();
}

template <typename T>
typename BlockVector<T>::reverse_iterator BlockVector<T>::rbegin() {
    return reverse_iterator(end());
}

template <typename T>
typename BlockVector<T>::const_reverse_iterator BlockVector<T>::rbegin() const {
    return const_reverse_iterator(end());
}

template <typename T>
typename BlockVector<T>::const_reverse_iterator BlockVector<T>::crbegin() const {
    return const_reverse_iterator(end());
}

template <typename T>
typename BlockVector<T>::reverse_iterator BlockVector<T>::rend() {
    return reverse_iterator(begin());
}

template <typename T>
typename BlockVector<T>::const_reverse_iterator BlockVector<T>::rend() const {
    return const_reverse_iterator(begin());
}

template <typename T>
typename BlockVector<T>::const_reverse_iterator BlockVector<T>::crend() const {
    return const_reverse_iterator(begin());
}
