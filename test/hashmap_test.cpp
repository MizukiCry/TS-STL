#include "src/hashmap.h"
#include "test_utils.h"
#include <gtest/gtest.h>

TEST(HashMapTest, FixedHashMapTest) {
  ts_stl::FixedHashMap<int, size_t> m1(1000);
  std::unordered_map<int, size_t> m2;

  size_t T = 100000;
  for (size_t i = 0; i < T; ++i) {
    size_t x = Random(0, T), y = Random();
    m1[x] = y;
    m2[x] = y;
  }

  for (size_t i = 0; i < T; ++i) {
    EXPECT_EQ(m1.Contains(i), m2.find(i) != m2.end());
    if (m1.Contains(i)) {
      EXPECT_EQ(m1[i], m2[i]);
    }
  }
}

TEST(HashMapTest, SyncFixedHashMapTest) {}

TEST(HashMapTest, HashMapTest) {
  return;
  ts_stl::HashMap<int, size_t> m1;
  std::unordered_map<int, size_t> m2;

  size_t T = 10000000;
  for (size_t i = 0; i < T; ++i) {
    size_t x = Random(0, T), y = Random();
    m1[x] = y;
    m2[x] = y;
  }

  for (size_t i = 0; i < T; ++i) {
    EXPECT_EQ(m1.Contains(i), m2.find(i) != m2.end());
    if (m1.Contains(i)) {
      EXPECT_EQ(m1[i], m2[i]);
    }
  }
}