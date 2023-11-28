#include "src/queue.h"
#include "test_utils.h"
#include <cstddef>
#include <future>
#include <gtest/gtest.h>

TEST(QueueTest, BasicTest) {
  ts_stl::Queue<int> q;
  ASSERT_EQ(q.size(), 0);
  ASSERT_TRUE(q.Empty());

  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(q.size(), i);
    q.Push(i);
  }

  int sum = 0;

  for (int i = 0; i < (int)q.size(); ++i) {
    ASSERT_EQ(q[i], i);
    sum += q[i];
  }

  ASSERT_EQ(sum, 49995000);

  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(q.Pop(), i);
  }

  ASSERT_TRUE(q.Empty());
  ASSERT_EQ(q.size(), 0);

  q.Clear();
  ASSERT_EQ(q.size(), 0);
  ASSERT_TRUE(q.Empty());

  for (int i = 0; i < 10000; ++i) {
    q.Push(i);
  }

  q.Clear();
  ASSERT_EQ(q.size(), 0);
  ASSERT_TRUE(q.Empty());
}

TEST(QueueTest, SyncTest) {
  for (int t = 0; t < 20; ++t) {
    ts_stl::SyncQueue<int> q;

    std::vector<std::future<uint64_t>> fs;

    for (int i = 0; i < 100; ++i) {
      fs.push_back(std::async([&q]() -> uint64_t {
        uint64_t sum = 0;
        for (int i = 0; i < 1000; ++i) {
          auto x = FastRandom(0, 0x7fffffff);
          q.Push(x);
          sum += x;
        }
        return sum;
      }));
    }

    uint64_t sum = 0, sumq = 0;
    for (auto &f : fs) {
      sum += f.get();
    }
    while (!q.Empty()) {
      sumq += q.Pop();
    }

    ASSERT_EQ(sum, sumq);
  }
}