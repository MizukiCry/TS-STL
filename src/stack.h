#ifndef TS_STL_STACK_H_
#define TS_STL_STACK_H_

#include "src/vector.h"
#include <mutex>

namespace ts_stl {
template <typename T> class Stack {
public:
  using value_type = T;
  using size_type = typename ts_stl::Vector<T>::size_type;

private:
  ts_stl::Vector<T> v_;

public:
  Stack() { v_.set_auto_shrink(true); }

  ~Stack() = default;

  Stack(const Stack &) = default;

  Stack(Stack &&) = default;

  auto operator=(const Stack &) -> Stack & = default;

  auto operator=(Stack &&) -> Stack & = default;

  void Push(const T &value) { v_.PushBack(value); }

  void Emplace(T &&value) { v_.EmplaceBack(std::move(value)); }

  auto Top() const -> T {
    Assert(!v_.Empty(), "Stack::Top(): stack is empty.");
    return v_.Back();
  }

  auto Pop() -> T {
    Assert(!v_.Empty(), "Stack::Pop(): stack is empty.");
    return v_.PopBack();
  }

  auto operator[](size_t index) -> T & {
    Assert(index < v_.size(), "Stack::operator[]: index out of range.");
    return v_[index];
  }

  auto operator[](size_t index) const -> const T & {
    Assert(index < v_.size(), "Stack::operator[]: index out of range.");
    return v_[index];
  }

  void Clear() { v_.Clear(); }

  auto Empty() const -> bool { return v_.Empty(); }

  auto Size() const -> size_t { return v_.size(); }

  auto Data() const -> Vector<T> { return v_; }
};

template <typename T> class SyncStack {
public:
  using value_type = T;
  using size_type = typename ts_stl::Stack<T>::size_type;

private:
  ts_stl::Stack<T> s_;
  std::mutex m_;

public:
  SyncStack() = default;

  ~SyncStack() = default;

  SyncStack(const SyncStack &) = default;

  SyncStack(SyncStack &&) = default;

  auto operator=(const SyncStack &) -> SyncStack & = default;

  auto operator=(SyncStack &&) -> SyncStack & = default;

  void Push(const T &value) {
    std::lock_guard<std::mutex> lock(m_);
    s_.PushBack(value);
  }

  void Emplace(T &&value) {
    std::lock_guard<std::mutex> lock(m_);
    s_.EmplaceBack(std::move(value));
  }

  auto Top() -> T {
    std::lock_guard<std::mutex> lock(m_);
    return s_.Back();
  }

  auto Top() const -> T { return s_.Back(); }

  auto Pop() -> T {
    std::lock_guard<std::mutex> lock(m_);
    return s_.PopBack();
  }

  auto operator[](size_t index) -> T {
    std::lock_guard<std::mutex> lock(m_);
    return s_[index];
  }

  auto operator[](size_t index) const -> T { return s_[index]; }

  void Clear() {
    std::lock_guard<std::mutex> lock(m_);
    s_.Clear();
  }

  auto Empty() -> bool {
    std::lock_guard<std::mutex> lock(m_);
    return s_.Empty();
  }

  auto Empty() const -> bool { return s_.Empty(); }

  auto Size() -> size_t {
    std::lock_guard<std::mutex> lock(m_);
    return s_.Size();
  }

  auto Size() const -> size_t { return s_.Size(); }

  auto RawStack() -> Stack<T> {
    std::lock_guard<std::mutex> lock(m_);
    return s_;
  }

  auto RawStack() const -> Stack<T> { return s_; }
};
} // namespace ts_stl

#endif