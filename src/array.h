#ifndef TS_STL_ARRAY_H_
#define TS_STL_ARRAY_H_

#include "src/utils.h"
#include <cstddef>

namespace ts_stl {
template <typename T, size_t S> class FixedArray {
public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;
  using iterator = T *;
  using const_iterator = const T *;

private:
  T _array[S];

public:
  FixedArray() = default;

  FixedArray(const FixedArray &other) {
    
  }

  ~FixedArray() = default;
};

template <typename T> class Array {
public:
private:
public:
};
} // namespace ts_stl

#endif