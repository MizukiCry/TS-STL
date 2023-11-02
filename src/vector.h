#ifndef TS_STL_VECTOR_H_
#define TS_STL_VECTOR_H_

#include "utils.h"
#include <cstddef>
#include <mutex>
#include <shared_mutex>

namespace ts_stl {
template <typename T> class Vector {
private:
  size_t size_ = 0;
  size_t capacity_ = 0;
  T *data_ = nullptr;

  // The multiple of each expansion
  double expand_factor_ = 2.0;

  // Whether shrink automatically
  bool auto_shrink_ = false;

  void ChangeCapacity(size_t capacity) {
    if (capacity == capacity_) {
      return;
    }
    T *new_data = new T[capacity];
    if (size_ > capacity) {
      size_ = capacity;
    }
    Copy(new_data, data_, data_ + size_);
    // std::copy(data_, data_ + size_, new_data);
    Swap(new_data, data_);
    delete[] new_data;
    capacity_ = capacity;
  }

  void CheckExpand() {
    if (size_ == capacity_) {
      ChangeCapacity(Max(capacity_, static_cast<size_t>(1)) * expand_factor_);
    }
  }

  void CheckShrink() {
    if (auto_shrink_ && size_ <= capacity_ / expand_factor_ / expand_factor_) {
      ChangeCapacity(size_ * expand_factor_);
    }
  }

public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = std::ptrdiff_t;

  using iterator = T *;
  using const_iterator = const T *;

  auto begin() -> iterator { return data_; }
  auto end() -> iterator { return data_ + size_; }
  auto begin() const -> const_iterator { return data_; }
  auto end() const -> const_iterator { return data_ + size_; }

  Vector() {}

  Vector(size_t size) : size_(size) { ChangeCapacity(size); }

  Vector(size_t size, const T &value) : size_(size) {
    ChangeCapacity(size);
    Fill(data_, data_ + size, value);
  }

  Vector(const Vector &other) = delete;
  Vector &operator=(const Vector &other) = delete;

  ~Vector() { delete[] data_; }

  void PushBack(const T &value) {
    CheckExpand();
    *(data_ + size_++) = value;
  }

  T PopBack() {
    Assert(size_ > 0, "Vector::PopBack(): vector is empty.");
    T t = *(data_ + --size_);
    CheckShrink();
    return t;
  }

  void Insert(size_t index, const T &value) {
    Assert(index <= size_, "Vector::Insert(): index out of range.");
    CheckExpand();
    CopyBackward(data_ + size_ + 1, data_ + index, data_ + size_);
    *(data_ + index) = value;
    size_++;
  }

  void Delete(size_t index) {
    Assert(index < size_, "Vector::Delete(): index out of range.");
    Copy(data_ + index, data_ + index + 1, data_ + size_);
    size_--;
    CheckShrink();
  }

  void Resize(size_t size) {
    if (capacity_ < size) {
      ChangeCapacity(size * expand_factor_);
    }
    size_ = size;
    CheckShrink();
  }

  void Reserve(size_t capacity) {
    if (capacity > capacity_) {
      ChangeCapacity(capacity);
    }
  }

  auto operator[](size_t index) -> T & {
    Assert(index < size_, "Vector::operator[]: index out of range.");
    return data_[index];
  }

  auto operator[](size_t index) const -> const T & {
    Assert(index < size_, "Vector::operator[]: index out of range.");
    return data_[index];
  }

  auto size() -> size_t const { return size_; }
  auto capacity() -> size_t const { return capacity_; }

  auto auto_shrink() -> bool const { return auto_shrink_; }
  void set_auto_shrink(bool auto_shrink) { auto_shrink_ = auto_shrink; }

  auto expand_factor() const -> double { return expand_factor_; }
  void set_expand_factor(double expand_factor) {
    expand_factor_ = expand_factor;
  }

  auto Empty() const -> bool { return size_ == 0; }

  void ShrinkToFit() { ChangeCapacity(size_); }

  void Clear() {
    size_ = 0;
    CheckShrink();
  }
};

template <typename T> class SyncVector : public Vector<T> {
private:
  std::shared_mutex m;

public:
  void ReadLock() { m.lock_shared(); }

  void WriteLock() { m.lock(); }

  void ReadUnlock() { m.unlock_shared(); }

  void WriteUnlock() { m.unlock(); }

  auto TryReadLock() -> bool { return m.try_lock_shared(); }

  auto TryWriteLock() -> bool { return m.try_lock(); }

  auto ReadLockGuard() -> std::shared_lock<std::shared_mutex> {
    return std::shared_lock<std::shared_mutex>(m);
  }

  auto WriteLockGuard() -> std::unique_lock<std::shared_mutex> {
    return std::unique_lock<std::shared_mutex>(m);
  }
};
} // namespace ts_stl

#endif