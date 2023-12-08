#ifndef TS_STL_UTILS_H_
#define TS_STL_UTILS_H_

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace {}

namespace ts_stl {

// SFINAE
template <typename T> class has_assignment_operator {
private:
  template <typename U>
  static auto test(U &&u)
      -> decltype(std::declval<U &>() = std::declval<U>(), std::true_type{});

  template <typename> static auto test(...) -> std::false_type;

public:
  static constexpr bool value = decltype(test<T>(std::declval<T>()))::value;
};

inline void Assert(bool condition, const char *message) {
  if (!condition) {
    std::cerr << message << std::endl;
    assert(false);
  }
}

inline void Todo() { Assert(false, "To be implemented."); }

template <typename T> auto Max(const T &a, const T &b) -> T {
  return a > b ? a : b;
}

template <typename T> auto Min(const T &a, const T &b) -> T {
  return a < b ? a : b;
}

template <typename T> auto Abs(const T &a) -> T { return a < 0 ? -a : a; }

template <typename T> auto Clamp(const T &v, const T &min, const T &max) -> T {
  return Max(min, Min(v, max));
}

template <typename T>
auto Between(const T &v, const T &min, const T &max) -> bool {
  return v >= min && v <= max;
}

template <typename T, typename U>
auto Lerp(const T &a, const T &b, const U &t) -> T {
  return a + (b - a) * t;
}

template <typename T, typename Iter>
void Fill(Iter begin, Iter end, const T &value) {
  for (; begin != end; begin++) {
    *begin = value;
  }
}

template <typename Iter1, typename Iter2>
auto Copy(Iter1 dest_begin, Iter2 begin, Iter2 end) -> Iter1 {
  return std::copy(begin, end, dest_begin);
}

template <typename Iter1, typename Iter2>
auto CopyBackward(Iter1 dest_end, Iter2 begin, Iter2 end) -> Iter1 {
  return std::copy_backward(begin, end, dest_end);
}

template <typename Iter1, typename Iter2>
auto ConstructorCopy(Iter1 dest_begin, Iter2 begin, Iter2 end) -> Iter1 {
  while (begin != end) {
    new (std::addressof(*dest_begin++))
        typename std::iterator_traits<Iter1>::value_type(std::move(*begin++));
  }
  return dest_begin;
}

template <typename Iter1, typename Iter2>
auto ConstructorCopyBackward(Iter1 dest_end, Iter2 begin, Iter2 end) -> Iter1 {
  while (begin != end) {
    new (std::addressof(*--dest_end))
        typename std::iterator_traits<Iter1>::value_type(std::move(*--end));
  }
  return dest_end;
}

template <typename Iter>
auto AutoCopy(Iter dest_begin, Iter begin, Iter end) -> Iter {
  if constexpr (has_assignment_operator<decltype(*Iter{})>::value) {
    return Copy(dest_begin, begin, end);
  } else {
    // static_assert(has_assignment_operator<decltype(*Iter{})>::value, "!");
    return ConstructorCopy(dest_begin, begin, end);
  }
}

template <typename Iter>
auto AutoCopyBackward(Iter dest_end, Iter begin, Iter end) -> Iter {
  if constexpr (has_assignment_operator<decltype(*Iter{})>::value) {
    return CopyBackward(dest_end, begin, end);
  } else {
    return ConstructorCopyBackward(dest_end, begin, end);
  }
}

template <typename T> void Swap(T &a, T &b) {
  T c = a;
  a = b;
  b = c;
}

inline auto time_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

inline auto time_us() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

inline auto time_ns() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

template <typename Iter, typename Pred>
auto AllOf(Iter first, Iter last, Pred pred) -> bool {
  // Todo
  return std::all_of(first, last, pred);
}

template <typename Iter, typename Pred>
auto AnyOf(Iter first, Iter last, Pred pred) -> bool {
  // Todo
  return std::any_of(first, last, pred);
}

template <typename Iter, typename Pred>
auto NoneOf(Iter first, Iter last, Pred pred) -> bool {
  // Todo
  return std::none_of(first, last, pred);
}

} // namespace ts_stl

#endif