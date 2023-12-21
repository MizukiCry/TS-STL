#include "src/map.h"
#include "test_utils.h"
#include <gtest/gtest.h>
#include <map>
#include <random>
#include <utility>
#include <vector>

TEST(MapTest, BasicTest) {
  ts_stl::Map<int, int> map1;
  ASSERT_EQ(map1.begin(), map1.end());

  map1.Insert(1, 10);
  map1.Insert(2, 20);
  map1.Insert(3, 30);

  ASSERT_EQ(map1.Size(), 3);

  ts_stl::Map<int, int> map2(map1);

  ASSERT_EQ(map1.Size(), map2.Size());

  for (auto i1 = map1.begin(), i2 = map2.begin(); i1 != map1.end();
       ++i1, ++i2) {
    ASSERT_EQ((*i1).first, (*i2).first);
    ASSERT_EQ((*i1).second, (*i2).second);
  }

  ts_stl::Map<int, int> map3(std::move(map2));

  ASSERT_EQ(map2.Size(), 0);
  ASSERT_EQ(map3.Size(), 3);

  for (auto i1 = map1.begin(), i3 = map3.begin(); i1 != map1.end();
       ++i1, ++i3) {
    ASSERT_EQ((*i1).first, (*i3).first);
    ASSERT_EQ((*i1).second, (*i3).second);
  }

  map1[4] = 40;
  ASSERT_EQ(map1.Size(), 4);
  ASSERT_EQ(map1[4], 40);

  map1.Delete(2);
  map1.Delete(3);
  ASSERT_EQ(map1.Size(), 2);
  ASSERT_TRUE(map1.Contains(1));
  ASSERT_FALSE(map1.Contains(2));
  ASSERT_FALSE(map1.Contains(3));
  ASSERT_TRUE(map1.Contains(4));
  ASSERT_EQ(map1[1], 10);
  ASSERT_EQ(map1[4], 40);
  ASSERT_EQ(map1.Find(1), map1.begin());
  ASSERT_EQ(map1.Find(2), map1.end());
  ASSERT_EQ(map1.Find(4), map1.back());
}

TEST(MapTest, RandomTest) {
  ts_stl::Map<size_t, size_t> map1;
  std::map<size_t, size_t> map2;

  std::vector<std::pair<size_t, size_t>> pairs;

  const int T = 1e5;
  for (int i = 0; i < T; ++i) {
    size_t key = Random();
    size_t value = Random();
    pairs.emplace_back(key, value);
    map1.Insert(key, value);
    map2.insert({key, value});
    ASSERT_EQ(map1.Size(), map2.size());
  }
  for (int i = 0; i < T; ++i) {
    size_t key = Random();
    size_t value = Random();
    pairs.emplace_back(key, value);
  }
  std::shuffle(pairs.begin(), pairs.end(),
               std::mt19937(std::random_device()()));
  for (auto [key, value] : pairs) {
    ASSERT_EQ(map1.Contains(key), map2.count(key) != 0);
    if (map1.Contains(key)) {
      ASSERT_EQ(map1[key], map2[key]);
    }
  }

  std::vector<std::pair<size_t, size_t>> v1, v2;
  for (auto [key, value] : map1) {
    v1.emplace_back(key, value);
  }
  for (auto [key, value] : map2) {
    v2.emplace_back(key, value);
  }
  ASSERT_EQ(v1, v2);
}