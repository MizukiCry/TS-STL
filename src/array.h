#ifndef TS_STL_ARRAY_H_
#define TS_STL_ARRAY_H_

#include "src/utils.h"
#include <cstddef>
#include <mutex>
#include <shared_mutex>

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
  value_type _array[S];

public:
  FixedArray() = default;

  FixedArray(const FixedArray<T, S> &other) {
    Copy(_array, other._array, other._array + S);
  }

  ~FixedArray() = default;

  auto operator=(const FixedArray<T, S> &other) -> FixedArray<T, S> & {
    Copy(_array, other._array, other._array + S);
    return *this;
  }

  auto begin() -> iterator { return _array; }
  auto begin() const -> const_iterator { return _array; }
  auto cbegin() const -> const_iterator { return _array; }
  auto end() -> iterator { return _array + S; }
  auto end() const -> const_iterator { return _array + S; }
  auto cend() const -> const_iterator { return _array + S; }

  static constexpr auto Size() -> size_type { return S; }

  auto Data() -> pointer { return _array; }

  auto Data() const -> const_pointer { return _array; }

  auto operator[](size_type index) -> reference {
    Assert(index < S, "Array::At(): index out of range.");
    return _array[index];
  }

  auto operator[](size_type index) const -> const_reference {
    Assert(index < S, "Array::At(): index out of range.");
    return _array[index];
  }
};

template <typename T, size_t S, size_t G = 1 /*Lock Granularity*/>
class SyncFixedArray {
  static_assert(G > 0,
                "SyncFixedArray: Lock granularity must be greater than 0.");

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
  FixedArray<value_type, S> _array;

  std::shared_mutex _global_m;
  std::shared_mutex _m[(S + G - 1) / G];

public:
  SyncFixedArray() = default;

  SyncFixedArray(const SyncFixedArray<value_type, S> &other)
      : _array(other._array) {}

  ~SyncFixedArray() = default;

  auto operator=(const SyncFixedArray<value_type, S> &other)
      -> SyncFixedArray<value_type, S> & {
    std::unique_lock<std::shared_mutex> lock(_global_m);
    _array = other._array;
    return *this;
  }

  static constexpr auto Size() -> size_type { return S; }

  auto RawArray() -> FixedArray<value_type, S> {
    std::unique_lock<std::shared_mutex> lock(_global_m);
    return _array;
  }

  auto RawArray() const -> FixedArray<value_type, S> { return _array; }

  auto operator[](size_type index) -> value_type {
    Assert(index < S, "Array::operator[]: index out of range.");
    std::shared_lock<std::shared_mutex> lock1(_global_m);
    std::shared_lock<std::shared_mutex> lock2(_m[index / G]);
    return _array[index];
  }

  auto operator[](size_type index) const -> value_type {
    Assert(index < S, "Array::operator[]: index out of range.");
    return _array[index];
  }

  void Set(size_type index, const value_type &value) {
    Assert(index < S, "Array::Set(): index out of range.");
    std::shared_lock<std::shared_mutex> lock1(_global_m);
    std::unique_lock<std::shared_mutex> lock2(_m[index / G]);
    _array[index] = value;
  }

  void Set(size_type index, value_type &&value) {
    Assert(index < S, "Array::Set(): index out of range.");
    std::shared_lock<std::shared_mutex> lock1(_global_m);
    std::unique_lock<std::shared_mutex> lock2(_m[index / G]);
    _array[index] = std::move(value);
  }
};

template <typename T> class Array {
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
  size_type _size;

  value_type *_array;

public:
  Array(size_type size = 0) : _size(size), _array(new value_type[size]) {}

  Array(const Array &other) : Array(other._size) {
    Copy(_array, other._array, other._array + other._size);
  }

  Array(Array &&other) : _size(other._size), _array(other._array) {
    other._array = nullptr;
  }

  ~Array() { delete[] _array; }

  auto operator=(const Array &other) -> Array & {
    _size = other._size;
    delete[] _array;
    _array = new value_type[_size];
    Copy(_array, other._array, other._array + _size);
  }

  auto operator=(Array &&other) -> Array & {
    _size = other._size;
    delete[] _array;
    _array = other._array;
    other._array = nullptr;
  }

  auto begin() -> iterator { return _array; }
  auto begin() const -> const_iterator { return _array; }
  auto cbegin() const -> const_iterator { return _array; }
  auto end() -> iterator { return _array + _size; }
  auto end() const -> const_iterator { return _array + _size; }
  auto cend() const -> const_iterator { return _array + _size; }

  auto size() const -> size_type { return _size; }

  auto Data() -> pointer { return _array; }

  auto Data() const -> const_pointer { return _array; }

  auto operator[](size_type index) -> reference { return _array[index]; }

  auto operator[](size_type index) const -> const_reference {
    return _array[index];
  }
};

template <typename T> class SyncArray {
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
  Array<value_type> _array;

  size_type _granularity;

  std::shared_mutex _global_m;

  Array<std::shared_mutex> _m;

public:
  SyncArray(size_type size = 0, size_type granularity = 1)
      : _array(size), _granularity(granularity),
        _m((size + granularity - 1) / granularity) {
    Assert(granularity > 0, "SyncArray: granularity must be positive.");
  }

  SyncArray(const SyncArray &other)
      : _array(other._array), _granularity(other._granularity),
        _m(other._m.size()) {}

  SyncArray(SyncArray &&other)
      : _array(std::move(other._array)), _granularity(other._granularity),
        _m(other._m.size()) {}

  ~SyncArray() = default;

  auto operator=(const SyncArray &other) -> SyncArray & {
    std::unique_lock<std::shared_mutex> lock(_global_m);
    _array = other._array;
    _granularity = other._granularity;
    _m = Array<std::shared_mutex>(other._m.size());
    return *this;
  }

  auto operator=(SyncArray &&other) -> SyncArray & {
    std::unique_lock<std::shared_mutex> lock(_global_m);
    _array = std::move(other._array);
    _granularity = other._granularity;
    _m = Array<std::shared_mutex>(other._m.size());
    return *this;
  }

  auto size() -> size_type {
    std::shared_lock<std::shared_mutex> lock(_global_m);
    return _array.size();
  }

  auto size() const -> size_type { return _array.size(); }

  auto granularity() -> size_type {
    std::shared_lock<std::shared_mutex> lock(_global_m);
    return _granularity;
  }

  auto granularity() const -> size_type { return _granularity; }

  auto RawArray() -> Array<value_type> {
    std::unique_lock<std::shared_mutex> lock(_global_m);
    return _array;
  }

  auto RawArray() const -> Array<value_type> { return _array; }

  auto operator[](size_type index) -> value_type {
    std::shared_lock<std::shared_mutex> lock1(_global_m);
    std::shared_lock<std::shared_mutex> lock2(_m[index / _granularity]);
    Assert(index < _array.size(), "SyncArray::operator[]: index out of range.");
    return _array[index];
  }

  auto operator[](size_type index) const -> const_reference {
    Assert(index < _array.size(), "SyncArray::operator[]: index out of range.");
    return _array[index];
  }

  void Set(size_type index, const value_type &value) {
    std::shared_lock<std::shared_mutex> lock1(_global_m);
    std::unique_lock<std::shared_mutex> lock2(_m[index / _granularity]);
    Assert(index < _array.size(), "SyncArray::Set(): index out of range.");
    _array[index] = value;
  }

  void Set(size_type index, value_type &&value) {
    std::shared_lock<std::shared_mutex> lock1(_global_m);
    std::unique_lock<std::shared_mutex> lock2(_m[index / _granularity]);
    Assert(index < _array.size(), "SyncArray::Set(): index out of range.");
    _array[index] = std::move(value);
  }
};
} // namespace ts_stl

#endif