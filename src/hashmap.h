#ifndef TS_STL_HASHMAP_H_
#define TS_STL_HASHMAP_H_

#include "src/vector.h"
#include <cstddef>
#include <iostream>

namespace ts_stl {

template <typename K, typename V, typename H> class FixedHashMap {
public:
  using key_type = K;
  using value_type = V;
  using size_type = std::size_t;

private:
const size_type _bucket_size;
  size_type _size;
  ts_stl::Vector<std::pair<K, V>> _bucket;

public:
  FixedHashMap(size_type bucket_size) : _bucket_size(bucket_size), _size(0) {
    _bucket.Resize(bucket_size);
  }



//   void Insert(const K& key, const )
};

template <typename K, typename V> class HashMap {
public:
  using key_type = K;
  using value_type = V;
  using size_type = std::size_t;

private:
public:
};
} // namespace ts_stl

#endif