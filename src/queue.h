#ifndef TS_STL_QUEUE_H_
#define TS_STL_QUEUE_H_

#include <cstddef>
namespace ts_stl {
template <typename T> class Queue {
public:
  using value_type = T;
  using size_type = std::size_t;

private:
public:
};

template <typename T> class SyncQueue {};
} // namespace ts_stl

#endif