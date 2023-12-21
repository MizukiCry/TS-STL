#ifndef TS_STL_QUEUE_H_
#define TS_STL_QUEUE_H_

#include "src/deque.h"
#include <mutex>
#include <shared_mutex>

namespace ts_stl {
template <typename T> class Queue {
public:
  using value_type = typename ts_stl::Deque<T>::value_type;
  using size_type = typename ts_stl::Deque<T>::size_type;
  using reference = typename ts_stl::Deque<T>::reference;
  using const_reference = typename ts_stl::Deque<T>::const_reference;

private:
  ts_stl::Deque<T> q_;

public:
  Queue() = default;

  Queue(const Queue &) = default;

  Queue(Queue &&) = default;

  auto operator=(const Queue &) -> Queue & = default;

  auto operator=(Queue &&) -> Queue & = default;

  ~Queue() = default;

  auto Front() -> reference { return q_.Front(); }

  auto Front() const -> const_reference { return q_.Front(); }

  auto Back() -> reference { return q_.Back(); }

  auto Back() const -> const_reference { return q_.Back(); }

  auto Empty() const -> bool { return q_.Empty(); }

  auto size() const -> size_type { return q_.size(); }

  void Push(const T &value) { q_.PushBack(value); }

  template <typename... Args> void Emplace(Args &&...args) {
    q_.EmplaceBack(std::forward<Args>(args)...);
  }

  auto Pop() -> value_type { return q_.PopFront(); }

  void Clear() { q_.Clear(); }

  auto operator[](size_type index) -> reference { return q_[index]; }

  auto operator[](size_type index) const -> const_reference {
    return q_[index];
  }
};

template <typename T> class SyncQueue {
public:
  using value_type = typename ts_stl::Deque<T>::value_type;
  using size_type = typename ts_stl::Deque<T>::size_type;
  using reference = typename ts_stl::Deque<T>::reference;
  using const_reference = typename ts_stl::Deque<T>::const_reference;

private:
  ts_stl::Deque<T> q_;
  std::shared_mutex m_;

public:
  SyncQueue() = default;

  SyncQueue(const SyncQueue &) = default;

  SyncQueue(SyncQueue &&other) = default;

  auto operator=(const SyncQueue &other) -> SyncQueue & {
    if (this != &other) {
      std::unique_lock<std::shared_mutex> lock(m_);
      q_ = other.q_;
    }
    return *this;
  }

  auto operator=(SyncQueue &&other) -> SyncQueue & {
    if (this != &other) {
      std::unique_lock<std::shared_mutex> lock(m_);
      q_ = std::move(other.q_);
    }
    return *this;
  }

  ~SyncQueue() = default;

  auto Front() -> value_type {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.Front();
  }

  auto Front() const -> value_type { return q_.Front(); }

  auto Back() -> value_type {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.Back();
  }

  auto Back() const -> value_type { return q_.Back(); }

  auto Empty() -> bool {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.Empty();
  }

  auto Empty() const -> bool { return q_.Empty(); }

  auto size() -> size_type {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_.size();
  }

  auto size() const -> size_type { return q_.size(); }

  void Push(const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.PushBack(value);
  }

  template <typename... Args> void Emplace(Args &&...args) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.EmplaceBack(std::forward<Args>(args)...);
  }

  auto Pop() -> value_type {
    std::unique_lock<std::shared_mutex> lock(m_);
    return q_.PopFront();
  }

  void Clear() {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_.Clear();
  }

  void Set(size_type index, const T &value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_[index] = value;
  }

  void Set(size_type index, const T &&value) {
    std::unique_lock<std::shared_mutex> lock(m_);
    q_[index] = std::move(value);
  }

  auto operator[](size_type index) -> value_type {
    std::shared_lock<std::shared_mutex> lock(m_);
    return q_[index];
  }

  auto operator[](size_type index) const -> value_type { return q_[index]; }
};
} // namespace ts_stl

#endif