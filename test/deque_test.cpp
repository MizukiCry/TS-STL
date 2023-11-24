#include "src/deque.h"
#include "test_utils.h"
#include <future>
#include <gtest/gtest.h>

TEST(DequeTest, BasicTest) {
  ts_stl::Deque<int> q;
  ASSERT_EQ(q.size(), 0);
  ASSERT_EQ(q.begin(), q.end());
  ASSERT_TRUE(q.Empty());

  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(q.size(), i);
    q.PushBack(i);
  }

  for (int i = 0; i < (int)q.size(); ++i) {
    ASSERT_EQ(q[i], i);
  }

  int sum = 0;
  for (int i : q) {
    sum += i;
  }
  ASSERT_EQ(sum, 49995000);

  for (int i = 9999; i >= 0; --i) {
    ASSERT_EQ(q.PopBack(), i);
  }

  ASSERT_TRUE(q.Empty());
  ASSERT_EQ(q.size(), 0);

  for (int i = 0; i < 10000; ++i) {
    q.PushBack(i);
    q.PushFront(i);
  }

  sum = 0;
  for (auto i : q) {
    sum += i;
  }

  ASSERT_EQ(sum, 2 * 49995000);

  q.Clear();
  ASSERT_EQ(q.size(), 0);
  ASSERT_TRUE(q.Empty());

  for (int i = 0; i < 10000; ++i) {
    q.PushBack(i);
  }

  q.Clear();
  ASSERT_EQ(q.size(), 0);
  ASSERT_EQ(q.MaxSize(), 0);
  ASSERT_TRUE(q.Empty());
}

TEST(DequeTest, InsertTest) {
  ts_stl::Deque<int> q;
  for (int i = 0; i < 100; ++i) {
    for (int j = 0; j < 100; ++j) {
      q.Insert((i + 1) * (j + 1) - 1, j * 100 + i);
    }
  }
  ASSERT_EQ(q.size(), 10000);
  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(q[i], i);
  }
  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(q.size(), 10000 - i);
    q.Delete(Random(0, q.size() - 1));
  }
  ASSERT_TRUE(q.Empty());
}

TEST(DequeTest, SyncTest) {
  for (int t = 0; t < 20; ++t) {
    ts_stl::SyncDeque<int> q;

    for (int i = 0; i < 100; ++i) {
      auto f = std::async([&]() {
        for (int j = 0; j < 100; ++j) {
          q.PushBack(j * 100 + i);
        }
      });
    }

    std::vector<std::future<int>> fs;
    for (int i = 0; i < 100; ++i) {
      fs.emplace_back(std::async([&]() -> int {
        int sum = 0;
        for (int j = 0; j < 100; ++j) {
          sum += q.PopBack();
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