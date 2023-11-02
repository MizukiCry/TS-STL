#ifndef TS_STL_STACK_H_
#define TS_STL_STACK_H_

#include "vector.h"
#include <shared_mutex>

namespace ts_stl {
template <typename T> class Stack {
public:
  using value_type = T;
  using size_type = typename ts_stl::Vector<T>::size_type;

private:
  ts_stl::Vector<T> v_;

public:
  Stack() { v_.set_auto_shrink(true); }
  ~Stack() {}

  Stack(const Stack &) = default;
  auto operator=(const Stack &) -> Stack & = default;

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

  auto Empty() const -> bool { return v_.Empty(); }

  auto Size() const -> size_t { return v_.size(); }
};

template <typename T> class SyncStack {
public:
  using value_type = T;
  using size_type = typename ts_stl::Vector<T>::size_type;

private:
  ts_stl::Vector<T> v_;

public:
  SyncStack() { v_.set_auto_shrink(true); }
  ~SyncStack() {}

  SyncStack(const SyncStack &) = default;
  auto operator=(const SyncStack &) -> SyncStack & {
    Todo();
  }

  void Push(const T &value) { v_.PushBack(value); }

  void Emplace(T &&value) { v_.EmplaceBack(std::move(value)); }

  auto Top() const -> T {
    Assert(!v_.Empty(), "SyncStack::Top(): stack is empty.");
    return v_.Back();
  }

  auto Pop() -> T {
    Assert(!v_.Empty(), "SyncStack::Pop(): stack is empty.");
    return v_.PopBack();
  }

  auto Empty() const -> bool { return v_.Empty(); }

  auto Size() const -> size_t { return v_.size(); }
};
} // namespace ts_stl

#endif