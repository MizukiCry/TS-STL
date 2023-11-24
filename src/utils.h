#ifndef TS_STL_UTILS_H_
#define TS_STL_UTILS_H_

#include <cassert>
#include <chrono>
#include <iostream>
#include <utility>

namespace {}

namespace ts_stl {
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
  Todo();
  while (begin != end)
    *dest_begin++ = std::move(*begin++);
  return dest_begin;
}

template <typename Iter1, typename Iter2>
auto CopyBackward(Iter1 dest_end, Iter2 begin, Iter2 end) -> Iter1 {
  return std::copy_backward(begin, end, dest_end);
  Todo();
  while (begin != end)
    *--dest_end = std::move(*--end);
  return dest_end;
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

} // namespace ts_stl

#endif