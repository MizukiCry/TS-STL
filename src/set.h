#ifndef TS_STL_SET_H_
#define TS_STL_SET_H_

#include "src/map.h"
#include "src/utils.h"
#include <cstddef>

namespace ts_stl {

template <typename T> class Set {
public:
  using value_type = T;
  using size_type = std::size_t;
  using reference = value_type &;
  using const_reference = const value_type &;

private:
  // Todo: Implement Set<T> = Map<T, void>
public:
  Set() { Assert(false, "Set: Unimplemented!"); }
};

template <typename T> class SyncSet {
public:
  using value_type = T;
  using size_type = std::size_t;
  using reference = value_type &;
  using const_reference = const value_type &;

private:
public:
  SyncSet() { Assert(false, "SyncSet: Unimplemented!"); }
};

} // namespace ts_stl

#endif