#ifndef TS_STL_HASHMAP_H_
#define TS_STL_HASHMAP_H_

#include "src/array.h"
#include "src/vector.h"
#include <atomic>
#include <cstddef>
#include <functional>
#include <utility>

namespace ts_stl {

template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class FixedHashMap {
public:
  using key_type = K;
  using value_type = V;
  using size_type = std::size_t;
  using reference = V &;
  using const_reference = const V &;

private:
  const size_type bucket_size_;

  size_type size_;

  ts_stl::Array<ts_stl::Vector<std::pair<key_type, value_type>>> bucket_;

  const Hash hasher_;

  const KeyEqual key_equaler_;

  auto BucketIndex(const key_type &key) const -> size_type {
    return hasher_(key) % bucket_size_;
  }

public:
  FixedHashMap(size_type bucket_size, Hash hasher = Hash(),
               KeyEqual key_equaler = KeyEqual())
      : bucket_size_(bucket_size), size_(0), bucket_(bucket_size),
        hasher_(hasher), key_equaler_(key_equaler) {}

  FixedHashMap(const FixedHashMap &) = default;

  FixedHashMap(FixedHashMap &&) = default;

  ~FixedHashMap() = default;

  auto operator=(const FixedHashMap &) -> FixedHashMap & = default;

  auto operator=(FixedHashMap &&) -> FixedHashMap & = default;

  auto size() const -> size_type { return size_; }

  auto Entry(const key_type &key) -> reference { return operator[](key); }

  auto Contains(const key_type &key) const -> bool {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return true;
      }
    }
    return false;
  }

  void Insert(const key_type &key, const value_type &value) {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        pair_value = value;
        return;
      }
    }
    bucket_[bucket_index].PushBack(std::make_pair(key, value));
    ++size_;
  }

  void Insert(const key_type &key, value_type &&value) {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        pair_value = std::move(value);
        return;
      }
    }
    bucket_[bucket_index].EmplaceBack(std::make_pair(key, std::move(value)));
    ++size_;
  }

  auto Delete(const key_type &key) -> bool {
    size_type bucket_index = BucketIndex(key);
    for (size_t index = 0; index < bucket_[bucket_index].size(); ++index) {
      if (key_equaler_(key, bucket_[bucket_index][index].first)) {
        bucket_[bucket_index].Delete(index);
        --size_;
        return true;
      }
    }
    return false;
  }

  auto operator[](const key_type &key) -> reference {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return pair_value;
      }
    }
    ++size_;
    bucket_[bucket_index].EmplaceBack(std::make_pair(key, value_type()));
    return bucket_[bucket_index].Back().second;
  }

  auto operator[](const key_type &key) const -> const_reference {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return pair_value;
      }
    }
    return value_type();
  }
};

template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class SyncFixedHashMap {
public:
  using key_type = K;
  using value_type = V;
  using size_type = std::size_t;
  using reference = V &;
  using const_reference = const V &;

private:
  const size_type bucket_size_;

  std::atomic<size_type> size_;

  ts_stl::Array<ts_stl::Vector<std::pair<key_type, value_type>>> bucket_;

  const Hash hasher_;

  const KeyEqual key_equaler_;

  ts_stl::Array<std::shared_mutex> m_;

  auto BucketIndex(const key_type &key) const -> size_type {
    return hasher_(key) % bucket_size_;
  }

public:
  SyncFixedHashMap(size_type bucket_size, Hash hasher = Hash(),
                   KeyEqual key_equaler = KeyEqual())
      : bucket_size_(bucket_size), size_(0), bucket_(bucket_size),
        hasher_(hasher), key_equaler_(key_equaler), m_(bucket_size) {}

  auto size() -> size_type { return size_; }

  auto Contains(const key_type &key) -> bool {
    size_type bucket_index = BucketIndex(key);
    std::shared_lock<std::shared_mutex> lock(m_[bucket_index]);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return true;
      }
    }
    return false;
  }

  auto Contains(const key_type &key) const -> bool {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return true;
      }
    }
    return false;
  }

  void Insert(const key_type &key, const value_type &value) {
    size_type bucket_index = BucketIndex(key);
    std::unique_lock<std::shared_mutex> lock(m_[bucket_index]);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        pair_value = value;
        return;
      }
    }
    ++size_;
    bucket_[bucket_index].Pushback(std::make_pair(key, value));
  }

  void Insert(const key_type &key, value_type &&value) {
    size_type bucket_index = BucketIndex(key);
    std::unique_lock<std::shared_mutex> lock(m_[bucket_index]);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        pair_value = std::move(value);
        return;
      }
    }
    ++size_;
    bucket_[bucket_index].EmplaceBack(std::make_pair(key, std::move(value)));
  }

  auto Delete(const key_type &key) -> bool {
    size_type bucket_index = BucketIndex(key);
    std::unique_lock<std::shared_mutex> lock(m_[bucket_index]);
    for (size_t index = 0; index < bucket_[bucket_index].size(); ++index) {
      if (key_equaler_(key, bucket_[bucket_index][index].first)) {
        bucket_[bucket_index].Delete(index);
        --size_;
        return true;
      }
    }
    return false;
  }

  auto operator[](const key_type &key) const -> value_type {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return pair_value;
      }
    }
    return value_type();
  }
};

template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class HashMap {
public:
  using key_type = K;
  using value_type = V;
  using size_type = std::size_t;
  using reference = V &;
  using const_reference = const V &;

private:
  ts_stl::Array<ts_stl::Vector<std::pair<key_type, value_type>>> bucket_;

  size_type bucket_size_;

  size_type size_;

  const double expand_factor_;

  const Hash hasher_;

  const KeyEqual key_equaler_;

  auto BucketIndex(const key_type &key) const -> size_type {
    return hasher_(key) % bucket_size_;
  }

public:
  HashMap(double expand_factor = 2.0, Hash hasher = Hash(),
          KeyEqual key_equaler = KeyEqual())
      : bucket_(1), bucket_size_(1), size_(0), expand_factor_(expand_factor),
        hasher_(hasher), key_equaler_(key_equaler) {
    Assert(expand_factor > 1.0,
           "HashMap: expand_factor must be greater than 1.0.");
  }

  HashMap(const HashMap &) = default;

  HashMap(HashMap &&) = default;

  ~HashMap() = default;

  auto operator=(const HashMap &) -> HashMap & = default;

  auto operator=(HashMap &&) -> HashMap & = default;

  void Resize(size_type new_bucket_size) {
    ts_stl::Array<ts_stl::Vector<std::pair<key_type, value_type>>> new_bucket(
        new_bucket_size);

    for (auto &bucket : bucket_) {
      for (auto &[pair_key, pair_value] : bucket) {
        new_bucket[hasher_(pair_key) % new_bucket_size].EmplaceBack(
            std::move(pair_key), std::move(pair_value));
      }
    }

    bucket_ = new_bucket;
    bucket_size_ = new_bucket_size;
  }

  auto bucket_size() const -> size_type { return bucket_size_; }

  auto size() const -> size_type { return size_; }

  auto Contains(const key_type &key) const -> bool {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return true;
      }
    }
    return false;
  }

  void Insert(const key_type &key, const value_type &value) {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        pair_value = value;
        return;
      }
    }
    bucket_[bucket_index].PushBack(std::make_pair(key, value));
    ++size_;
    if (size_ > bucket_size_ * expand_factor_) {
      Resize(size_);
    }
  }

  void Insert(const key_type &key, value_type &&value) {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        pair_value = std::move(value);
        return;
      }
    }
    bucket_[bucket_index].EmplaceBack(std::make_pair(key, std::move(value)));
    ++size_;
    if (size_ > bucket_size_ * expand_factor_) {
      Resize(size_);
    }
  }

  auto Delete(const key_type &key) -> bool {
    bool deleted = false;
    size_type bucket_index = BucketIndex(key);
    for (size_t index = 0; index < bucket_[bucket_index].size(); ++index) {
      if (key_equaler_(key, bucket_[bucket_index][index].first)) {
        bucket_[bucket_index].Delete(index);
        --size_;
        deleted = true;
        break;
      }
    }
    if (size_ * expand_factor_ * expand_factor_ < bucket_size_ && size_ > 4) {
      Resize(size_ * expand_factor_);
    }
    return deleted;
  }

  auto operator[](const key_type &key) -> reference {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return pair_value;
      }
    }
    ++size_;
    if (size_ > bucket_size_ * expand_factor_) {
      Resize(size_);
    }
    bucket_index = BucketIndex(key);
    bucket_[bucket_index].EmplaceBack(std::make_pair(key, value_type()));
    return bucket_[bucket_index].Back().second;
  }

  auto operator[](const key_type &key) const -> const_reference {
    size_type bucket_index = BucketIndex(key);
    for (auto &[pair_key, pair_value] : bucket_[bucket_index]) {
      if (key_equaler_(key, pair_key)) {
        return pair_value;
      }
    }
    return value_type();
  }
};

template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class SyncHashMap {

public:
  using key_type = K;
  using value_type = V;
  using size_type = std::size_t;
  using reference = V &;
  using const_reference = const V &;

private:
public:
  SyncHashMap() { Assert(false, "SyncHashMap: Unimplemented!"); }

  ~SyncHashMap() = default;
};
} // namespace ts_stl

#endif