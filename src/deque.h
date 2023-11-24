#ifndef TS_STL_DEQUE_H_
#define TS_STL_DEQUE_H_

#include "utils.h"
#include <cstddef>
#include <mutex>
#include <shared_mutex>

namespace ts_stl {
template <typename T> class Deque {
public:
  class iterator;

  using value_type = T;
  using size_type = std::size_t;
  using different_type = std::ptrdiff_t;
  using const_iterator = const iterator;

private:
  T *data_ = nullptr;
  size_type size_ = 0;
  size_type capacity_ = 0;
  size_type front_ = 0;
  size_type back_ = 0;

public:
  void Resize(size_type new_capacity) {
    T *new_data = new T[new_capacity];
    size_type new_size = Min(size_, new_capacity);
    if (front_ + new_size <= capacity_) {
      Copy(new_data, data_ + front_, data_ + front_ + new_size);
    } else {
      auto t = Copy(new_data, data_ + front_, data_ + capacity_);
      Copy(t, data_, data_ + front_ + new_size - capacity_);
    }
    delete[] data_;
    data_ = new_data;
    size_ = new_size;
    capacity_ = new_capacity;
    front_ = 0;
    back_ = size_;
  }

  // void Debug() {
  //   std::cerr << "size_: " << size_ << std::endl;
  //   std::cerr << "capacity_: " << capacity_ << std::endl;
  //   std::cerr << "front_: " << front_ << std::endl;
  //   std::cerr << "back_: " << back_ << std::endl;
  //   for (auto i = 0; i < capacity_; i++) {
  //     std::cerr << data_[i] << ' ';
  //   }
  //   std::cerr << std::endl;
  // }

private:
  inline void CheckExpand() {
    if (size_ == capacity_) {
      Resize(Max(static_cast<size_type>(1), capacity_ * 2));
    }
  }

  inline void CheckShrink() {
    if (size_ <= capacity_ / 4 && capacity_ >= 8) {
      Resize(capacity_ / 2);
    }
  }

  inline auto Index(size_type index) -> size_type {
    if (index < capacity_) {
      return index;
    }
    if (index < capacity_ * 2) {
      return index - capacity_;
    }
    return index + capacity_;
  }

public:
  class iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

  private:
    Deque<T> *deque_ = nullptr;

    // Offset to deque_.front_
    size_t pos_ = 0;

  public:
    iterator(Deque<T> *deque, size_t pos) : deque_(deque), pos_(pos) {}

    iterator(const iterator &other) = default;

    auto operator=(const iterator &other) -> iterator & = default;

    auto operator++() -> iterator & {
      ++pos_;
      Assert(pos_ <= deque_->size_, "Deque::iterator: iterator out of range");
      return *this;
    }

    auto operator++(int) -> iterator {
      auto ret = *this;
      ++pos_;
      Assert(pos_ <= deque_->size_, "Deque::iterator: iterator out of range");
      return ret;
    }

    auto operator--() -> iterator & {
      Assert(pos_ > 0, "Deque::iterator: iterator out of range");
      --pos_;
      return *this;
    }

    auto operator--(int) -> iterator {
      auto ret = *this;
      Assert(pos_ > 0, "Deque::iterator: iterator out of range");
      --pos_;
      return ret;
    }

    auto operator+=(difference_type n) -> iterator & {
      pos_ += n;
      Assert(pos_ <= deque_->size_, "Deque::iterator: iterator out of range");
      return *this;
    }

    auto operator-=(difference_type n) -> iterator & {
      pos_ -= n;
      Assert(pos_ <= deque_->size_, "Deque::iterator: iterator out of range");
      return *this;
    }

    auto operator-(const iterator &other) const -> difference_type {
      return pos_ - other.pos_;
    }

    auto operator+(difference_type n) const -> iterator {
      auto ret = *this;
      ret += n;
      return ret;
    }

    auto operator-(difference_type n) const -> iterator {
      auto ret = *this;
      ret -= n;
      return ret;
    }

    auto operator==(const iterator &other) const -> bool {
      return deque_ == other.deque_ && deque_ != nullptr && pos_ == other.pos_;
    }

    auto operator!=(const iterator &other) const -> bool {
      return !(*this == other);
    }

    auto operator<(const iterator &other) const -> bool {
      return pos_ < other.pos_;
    }

    auto operator>(const iterator &other) const -> bool {
      return pos_ > other.pos_;
    }

    auto operator<=(const iterator &other) const -> bool {
      return pos_ <= other.pos_;
    }

    auto operator>=(const iterator &other) const -> bool {
      return pos_ >= other.pos_;
    }

    auto operator*() const -> reference {
      Assert(deque_ != nullptr, "Deque::iterator: deque is nullptr");
      Assert(pos_ < deque_->size_, "Deque::iterator: iterator out of range");
      return (*deque_)[pos_];
    }
  };

  Deque() = default;

  Deque(const Deque<T> &other) {
    data_ = new T[other.capacity_];
    Copy(data_, other.data_, other.data_ + other.capacity_);
    size_ = other.size_;
    capacity_ = other.capacity_;
    front_ = other.front_;
    back_ = other.back_;
  }

  Deque(Deque<T> &&other) {
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    front_ = other.front_;
    back_ = other.back_;
    other.data_ = nullptr;
  }

  auto operator=(const Deque<T> &other) -> Deque<T> & {
    if (this == &other) {
      return *this;
    }
    delete[] data_;
    data_ = new T[other.capacity_];
    Copy(data_, other.data_, other.data_ + other.capacity_);
    size_ = other.size_;
    capacity_ = other.capacity_;
    front_ = other.front_;
    back_ = other.back_;
    return *this;
  }

  auto operator=(Deque<T> &&other) -> Deque<T> & {
    if (this == &other) {
      return *this;
    }
    delete[] data_;
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    front_ = other.front_;
    back_ = other.back_;
    other.data_ = nullptr;
    return *this;
  }

  ~Deque() { delete[] data_; }

  auto begin() -> iterator { return iterator(this, 0); }

  auto end() -> iterator { return iterator(this, size_); }

  auto begin() const -> const_iterator { return const_iterator(this, 0); }

  auto end() const -> const_iterator { return const_iterator(this, size_); }

  auto cbegin() const -> const_iterator { return const_iterator(this, 0); }

  auto cend() const -> const_iterator { return const_iterator(this, size_); }

  void PushFront(const T &value) {
    CheckExpand();
    data_[front_ = Index(front_ - 1)] = value;
    size_++;
  }

  template <typename... Args> void EmplaceFront(Args &&...args) {
    CheckExpand();
    new (&data_[front_ = Index(front_ - 1)]) T(std::forward<Args>(args)...);
    size_++;
  }

  void PushBack(const T &value) {
    CheckExpand();
    data_[back_] = value;
    back_ = Index(back_ + 1);
    size_++;
  }

  template <typename... Args> void EmplaceBack(Args &&...args) {
    CheckExpand();
    new (&data_[back_]) T(std::forward<Args>(args)...);
    back_ = Index(back_ + 1);
    size_++;
  }

  auto size() const -> size_type { return size_; }

  auto MaxSize() -> size_type { return capacity_; }

  auto Empty() const -> bool { return size_ == 0; }

  auto Front() -> T & {
    Assert(!Empty(), "Deque::Front(): deque is empty.");
    return data_[front_];
  }

  auto Front() const -> const T & {
    Assert(!Empty(), "Deque::Front(): deque is empty.");
    return data_[front_];
  }

  auto Back() -> T & {
    Assert(!Empty(), "Deque::Back(): deque is empty.");
    return data_[Index(back_ - 1)];
  }

  auto Back() const -> const T & {
    Assert(!Empty(), "Deque::Back(): deque is empty.");
    return data_[Index(back_ - 1)];
  }

  auto PopFront() -> T {
    Assert(!Empty(), "Deque::PopFront(): deque is empty.");
    front_ = Index(front_ + 1);
    size_--;
    T value = data_[Index(front_ - 1)];
    CheckShrink();
    return value;
  }

  auto PopBack() -> T {
    Assert(!Empty(), "Deque::PopBack(): deque is empty.");
    back_ = Index(back_ - 1);
    size_--;
    T value = data_[back_];
    CheckShrink();
    return value;
  }

  void Insert(size_type index, const T &value) {
    Assert(index <= size_, "Deque::Insert(): index out of range.");
    CheckExpand();
    if (index = Index(front_ + index); index <= back_) {
      CopyBackward(data_ + back_ + 1, data_ + index, data_ + back_);
      data_[index] = value;
    } else {
      CopyBackward(data_ + back_ + 1, data_, data_ + back_);
      data_[0] = std::move(data_[capacity_ - 1]);
      CopyBackward(data_ + capacity_, data_ + index, data_ + capacity_ - 1);
      data_[index] = value;
    }
    size_++;
    back_ = Index(back_ + 1);
  }

  template <typename... Args> void Emplace(size_type index, Args &&...args) {
    Assert(index <= size_, "Deque::Insert(): index out of range.");
    CheckExpand();
    if (index = Index(front_ + index); index <= back_) {
      CopyBackward(data_ + back_ + 1, data_ + index, data_ + back_);
      new (&data_[index]) T(std::forward<Args>(args)...);
    } else {
      CopyBackward(data_ + back_ + 1, data_, data_ + back_);
      data_[0] = std::move(data_[capacity_ - 1]);
      CopyBackward(data_ + capacity_, data_ + index, data_ + capacity_ - 1);
      new (&data_[index]) T(std::forward<Args>(args)...);
    }
    size_++;
    back_ = Index(back_ + 1);
  }

  void Delete(size_type index) {
    Assert(index < size_, "Deque::Delete(): index out of range.");
    if (index = Index(front_ + index); index < back_) {
      Copy(data_ + index, data_ + index + 1, data_ + back_);
    } else {
      Copy(data_ + index, data_ + index + 1, data_ + capacity_);
      data_[capacity_ - 1] = std::move(data_[0]);
      Copy(data_, data_ + 1, data_ + back_);
    }
    size_--;
    back_ = Index(back_ - 1);
    CheckShrink();
  }

  auto operator[](const size_type index) -> T & {
    Assert(index < size_, "Deque::operator[]: index out of range.");
    return data_[Index(front_ + index)];
  }

  auto operator[](const size_type index) const -> const T & {
    Assert(index < size_, "Deque::operator[]: index out of range.");
    return data_[Index(front_ + index)];
  }

  auto At(const size_type index) -> T & {
    Assert(index < size_, "Deque::At(): index out of range.");
    return data_[Index(front_ + index)];
  }

  auto At(const size_type index) const -> const T & {
    Assert(index < size_, "Deque::At(): index out of range.");
    return data_[Index(front_ + index)];
  }

  void ShrinkToFit() { Resize(size_); }

  void Clear() { Resize(0); }
};

template <typename T> class SyncDeque {
public:
  using value_type = T;
  using size_type = std::size_t;
  using different_type = std::ptrdiff_t;
  using iterator = typename ts_stl::Deque<T>::iterator;
  using const_iterator = const iterator;

private:
  Deque<T> q_;
  std::shared_mutex m_;

public:
  SyncDeque() = default;

  SyncDeque(const SyncDeque &other) : q_(other.q_) {}

  SyncDeque(SyncDeque &&other) : q_(std::move(other.q_)) {}

  ~SyncDeque() = default;

  auto operator=(const SyncDeque &other) -> SyncDeque & {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_ = other.q_;
    return *this;
  }

  auto operator=(SyncDeque &&other) -> SyncDeque & {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_ = std::move(other.q_);
    return *this;
  }

  auto begin() -> iterator { return q_.begin(); }

  auto begin() const -> const_iterator { return q_.begin(); }

  auto cbegin() const -> const_iterator { return q_.cbegin(); }

  auto end() -> iterator { return q_.end(); }

  auto end() const -> const_iterator { return q_.end(); }

  auto cend() const -> const_iterator { return q_.cend(); }

  void Resize(size_type new_capacity) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.Resize(new_capacity);
  }

  void PushFront(const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.PushFront(value);
  }

  template <typename... Args> void EmplaceFront(Args &&...args) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.EmplaceFront(std::forward<Args>(args)...);
  }

  void PushBack(const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.PushBack(value);
  }

  template <typename... Args> void EmplaceBack(Args &&...args) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.EmplaceBack(std::forward<Args>(args)...);
  }

  void Set(size_type index, const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_[index] = value;
  }

  void Set(size_type index, const T &&value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_[index] = std::move(value);
  }

  auto operator[](const size_type index) -> T {
    std::unique_lock<std::shared_mutex> lock(m_);
    return q_[index];
  }

  auto operator[](const size_type index) const -> const T { return q_[index]; }

  auto size() -> size_type {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.size();
  }

  auto size() const -> size_type { return q_.size(); }

  auto MaxSize() -> size_type {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.MaxSize();
  }

  auto MaxSize() const -> size_type { return q_.MaxSize(); }

  auto Empty() -> bool {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.Empty();
  }

  auto Empty() const -> bool { return q_.Empty(); }

  auto Front() -> T {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.Front();
  }

  auto Front() const -> T { return q_.Front(); }

  auto Back() -> T {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.Back();
  }

  auto Back() const -> T { return q_.Back(); }

  auto PopFront() -> T {
    std::unique_lock<std::shared_mutex> lock(m_);
    return q_.PopFront();
  }

  auto PopBack() -> T {
    std::unique_lock<std::shared_mutex> lock(m_);
    return q_.PopBack();
  }

  void Insert(size_type index, const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.Insert(index, value);
  }

  template <typename... Args> void Emplace(size_type index, Args &&...args) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.Emplace(index, std::forward<Args>(args)...);
  }

  void Delete(size_type index) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.Delete(index);
  }

  void ShrinkToFit() {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.ShrinkToFit();
  }

  void Clear() {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.Clear();
  }
};
} // namespace ts_stl

#endif