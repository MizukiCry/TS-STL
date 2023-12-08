#ifndef TS_STL_VECTOR_H_
#define TS_STL_VECTOR_H_

#include "src/utils.h"
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <utility>

namespace ts_stl {
template <typename T> class Vector {
public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  using iterator = T *;
  using const_iterator = const T *;

private:
  size_type size_ = 0;
  size_type capacity_ = 0;
  T *data_ = nullptr;

  // The multiple of each expansion
  double expand_factor_ = 2.0;

  // Whether shrink automatically
  bool auto_shrink_ = false;

  void ChangeCapacity(size_type capacity) {
    if (capacity == capacity_) {
      return;
    }
    T *new_data = new T[capacity];
    if (size_ > capacity) {
      size_ = capacity;
    }
    AutoCopy(new_data, data_, data_ + size_);
    Swap(new_data, data_);
    delete[] new_data;
    capacity_ = capacity;
  }

  void CheckExpand() {
    if (size_ == capacity_) {
      ChangeCapacity(Max(capacity_, static_cast<size_type>(1)) *
                     expand_factor_);
    }
  }

  void CheckShrink() {
    if (auto_shrink_ && size_ <= capacity_ / expand_factor_ / expand_factor_) {
      ChangeCapacity(size_ * expand_factor_);
    }
  }

public:
  auto begin() -> iterator { return data_; }
  auto end() -> iterator { return data_ + size_; }
  auto begin() const -> const_iterator { return data_; }
  auto end() const -> const_iterator { return data_ + size_; }
  auto cbegin() const -> const_iterator { return data_; }
  auto cend() const -> const_iterator { return data_ + size_; }

  Vector() {}

  Vector(size_type size) : size_(size) { ChangeCapacity(size); }

  Vector(size_type size, const T &value) : size_(size) {
    ChangeCapacity(size);
    Fill(data_, data_ + size, value);
  }

  Vector(const Vector &other)
      : size_(other.size_), capacity_(other.capacity_),
        expand_factor_(other.expand_factor_), auto_shrink_(other.auto_shrink_) {
    data_ = new value_type[capacity_];
    AutoCopy(data_, other.data_, other.data_ + size_);
  }

  Vector(Vector &&other) {
    size_ = other.size_;
    capacity_ = other.capacity_;
    expand_factor_ = other.expand_factor_;
    auto_shrink_ = other.auto_shrink_;
    data_ = other.data_;
    other.data_ = nullptr;
  }

  auto operator=(const Vector &other) -> Vector & {
    if (this != other) {
      size_ = other.size_;
      capacity_ = other.capacity_;
      expand_factor_ = other.expand_factor_;
      auto_shrink_ = other.auto_shrink_;
      delete[] data_;
      data_ = new value_type[capacity_];
      AutoCopy(data_, other.data_, other.data_ + size_);
    }
    return *this;
  }

  auto operator=(Vector &&other) -> Vector & {
    if (this != other) {
      size_ = other.size_;
      capacity_ = other.capacity_;
      expand_factor_ = other.expand_factor_;
      auto_shrink_ = other.auto_shrink_;
      data_ = other.data_;
      other.data_ = nullptr;
    }
    return *this;
  }

  ~Vector() { delete[] data_; }

  void PushBack(const T &value) {
    CheckExpand();
    *(data_ + size_++) = value;
  }

  template <typename... Args> void EmplaceBack(Args &&...args) {
    CheckExpand();
    new (data_ + size_++) T(std::forward<Args>(args)...);
  }

  auto PopBack() -> T {
    Assert(size_ > 0, "Vector::PopBack(): vector is empty.");
    T t = *(data_ + --size_);
    CheckShrink();
    return t;
  }

  auto Back() -> T & {
    Assert(size_ > 0, "Vector::Back(): vector is empty.");
    return *(data_ + size_ - 1);
  }

  auto Back() const -> const T & {
    Assert(size_ > 0, "Vector::Back(): vector is empty.");
    return *(data_ + size_ - 1);
  }

  void Insert(size_type index, const T &value) {
    Assert(index <= size_, "Vector::Insert(): index out of range.");
    CheckExpand();
    AutoCopyBackward(data_ + size_ + 1, data_ + index, data_ + size_);
    *(data_ + index) = value;
    size_++;
  }

  template <typename... Args> void Emplace(size_type index, Args &&...args) {
    Assert(index <= size_, "Vector::Emplace(): index out of range.");
    CheckExpand();
    AutoCopyBackward(data_ + size_ + 1, data_ + index, data_ + size_);
    new (data_ + index) T(std::forward<Args>(args)...);
    size_++;
  }

  auto Delete(size_type index) -> T {
    Assert(index < size_, "Vector::Delete(): index out of range.");
    T t = *(data_ + index);
    AutoCopy(data_ + index, data_ + index + 1, data_ + size_);
    size_--;
    CheckShrink();
    return t;
  }

  void Resize(size_type size) {
    if (capacity_ < size) {
      ChangeCapacity(size * expand_factor_);
    }
    size_ = size;
    CheckShrink();
  }

  void Reserve(size_type capacity) {
    if (capacity > capacity_) {
      ChangeCapacity(capacity);
    }
  }

  auto operator[](size_type index) -> T & {
    Assert(index < size_, "Vector::operator[]: index out of range.");
    return data_[index];
  }

  auto operator[](size_type index) const -> const T & {
    Assert(index < size_, "Vector::operator[]: index out of range.");
    return data_[index];
  }

  auto size() const -> size_type { return size_; }
  auto capacity() const -> size_type { return capacity_; }

  auto auto_shrink() const -> bool { return auto_shrink_; }
  void set_auto_shrink(bool auto_shrink) { auto_shrink_ = auto_shrink; }

  auto expand_factor() const -> double { return expand_factor_; }
  void set_expand_factor(double expand_factor) {
    expand_factor_ = expand_factor;
  }

  auto RawData() -> T * { return data_; }
  auto RawData() const -> const T * { return data_; }

  auto Empty() const -> bool { return size_ == 0; }

  void ShrinkToFit() { ChangeCapacity(size_); }

  void Clear() {
    size_ = 0;
    CheckShrink();
  }
};

template <typename T> class SyncVector {
public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  using iterator = T *;
  using const_iterator = const T *;

private:
  Vector<T> v_;
  std::shared_mutex m_;

public:
  auto begin() -> iterator { return v_.begin(); }
  auto end() -> iterator { return v_.end(); }
  auto begin() const -> const_iterator { return v_.begin(); }
  auto end() const -> const_iterator { return v_.end(); }
  auto cbegin() const -> const_iterator { return v_.cbegin(); }
  auto cend() const -> const_iterator { return v_.cend(); }

  SyncVector() {}

  SyncVector(size_type size) : v_(size) {}

  SyncVector(size_type size, const T &value) : v_(size, value) {}

  SyncVector(const SyncVector &other) : v_(other.v_) {}

  SyncVector(SyncVector &&other) : v_(std::move(other.v_)) {}

  ~SyncVector() = default;

  SyncVector &operator=(const SyncVector &other) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_ = other.v_;
    return *this;
  }

  SyncVector &operator=(SyncVector &&other) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_ = std::move(other.v_);
    return *this;
  }

  void PushBack(const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.PushBack(value);
  }

  template <typename... Args> void EmplaceBack(Args &&...args) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.EmplaceBack(std::forward<Args>(args)...);
  }

  auto PopBack() -> T {
    std::unique_lock<std::shared_mutex> lock(m_);
    return v_.PopBack();
  }

  auto Back() -> T & {
    std::unique_lock<std::shared_mutex> lock(m_);
    return v_.Back();
  }

  auto Back() const -> const T & { return v_.Back(); }

  void Insert(size_type index, const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.Insert(index, value);
  }

  template <typename... Args> void Emplace(size_type index, Args &&...args) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.Emplace(index, std::forward<Args>(args)...);
  }

  auto Delete(size_type index) -> T {
    std::unique_lock<std::shared_mutex> lock(m_);
    return v_.Delete(index);
  }

  void Resize(size_type size) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.Resize(size);
  }

  void Reserve(size_type capacity) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.Reserve(capacity);
  }

  auto operator[](size_type index) -> T {
    std::shared_lock<std::shared_mutex> lock(m_);
    return v_[index];
  }

  auto operator[](size_type index) const -> T { return v_[index]; }

  void Set(size_type index, const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_[index] = value;
  }

  void Set(size_type index, const T &&value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_[index] = std::move(value);
  }

  auto size() -> size_type {
    std::shared_lock<std::shared_mutex> lock(m_);
    return v_.size();
  }
  auto size() const -> size_type { return v_.size(); }
  auto capacity() -> size_type {
    std::shared_lock<std::shared_mutex> lock(m_);
    return v_.capacity();
  }
  auto capacity() const -> size_type { return v_.capacity(); }

  auto auto_shrink() -> bool {
    std::shared_lock<std::shared_mutex> lock(m_);
    return v_.auto_shrink();
  }
  auto auto_shrink() const -> bool { return v_.auto_shrink(); }
  void set_auto_shrink(bool auto_shrink) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.set_auto_shrink(auto_shrink);
  }

  auto expand_factor() -> double {
    std::shared_lock<std::shared_mutex> lock(m_);
    return v_.expand_factor();
  }
  auto expand_factor() const -> double { return v_.expand_factor(); }
  void set_expand_factor(double expand_factor) {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.set_expand_factor(expand_factor);
  }

  auto Empty() -> bool {
    std::shared_lock<std::shared_mutex> lock(m_);
    return v_.empty();
  }
  auto Empty() const -> bool { return v_.empty(); }

  void ShrinkToFit() {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.ShrinkToFit();
  }

  void Clear() {
    std::unique_lock<std::shared_mutex> lock(m_);
    v_.Clear();
  }

  auto RawVector() -> Vector<T> {
    std::shared_lock<std::shared_mutex> lock(m_);
    return v_;
  }
  auto RawVector() const -> Vector<T> { return v_; }
};
} // namespace ts_stl

#endif