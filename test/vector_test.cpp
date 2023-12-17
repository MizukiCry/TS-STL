#include "src/vector.h"
#include "test_utils.h"
#include <future>
#include <gtest/gtest.h>

TEST(VectorTest, BasicTest) {
  ts_stl::Vector<int> v;
  v = ts_stl::Vector<int>();
  ASSERT_EQ(v.size(), 0);
  ASSERT_EQ(v.begin(), v.end());
  ASSERT_TRUE(v.Empty());

  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(v.size(), i);
    v.PushBack(i);
    ASSERT_LE(v.capacity(), v.size() * v.expand_factor() * v.expand_factor());
  }

  for (int i = 0; i < (int)v.size(); ++i) {
    ASSERT_EQ(v[i], i);
  }

  int sum = 0;
  for (int i : v) {
    sum += i;
  }
  ASSERT_EQ(sum, 49995000);

  for (int i = 9999; i >= 0; --i) {
    ASSERT_EQ(v.PopBack(), i);
  }

  ASSERT_TRUE(v.Empty());
  ASSERT_EQ(v.size(), 0);

  for (int i = 0; i < 10000; ++i) {
    v.PushBack(i);
  }

  v.Clear();
  ASSERT_EQ(v.size(), 0);
  ASSERT_TRUE(v.Empty());

  for (int i = 0; i < 10000; ++i) {
    v.PushBack(i);
  }

  v.set_auto_shrink(true);

  v.Clear();
  ASSERT_EQ(v.size(), 0);
  ASSERT_EQ(v.capacity(), 0);
  ASSERT_TRUE(v.Empty());
}

TEST(VectorTest, InsertTest) {
  ts_stl::Vector<int> v;
  for (int i = 0; i < 100; ++i) {
    for (int j = 0; j < 100; ++j) {
      v.Insert((i + 1) * (j + 1) - 1, j * 100 + i);
    }
  }
  ASSERT_EQ(v.size(), 10000);
  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(v[i], i);
  }
  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(v.size(), 10000 - i);
    v.Delete(Random(0, v.size() - 1));
  }
  ASSERT_TRUE(v.Empty());
}

TEST(VectorTest, SyncTest) {
  for (int t = 0; t < 20; ++t) {
    ts_stl::SyncVector<int> v;

    for (int i = 0; i < 100; ++i) {
      auto f = std::async([&]() {
        for (int j = 0; j < 100; ++j) {
          v.PushBack(j * 100 + i);
        }
      });
    }

    std::vector<std::future<int>> fs;
    for (int i = 0; i < 100; ++i) {
      fs.emplace_back(std::async([&]() -> int {
        int sum = 0;
        for (int j = 0; j < 100; ++j) {
          sum += v.PopBack();
        }
        return sum;
      }));
    }
    int sum = 0;
    for (auto &f : fs) {
      sum += f.get();
    }

    ASSERT_EQ(sum, 49995000);
  }
}