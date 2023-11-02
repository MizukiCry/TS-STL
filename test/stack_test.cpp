#include "src/stack.h"
#include <future>
#include <gtest/gtest.h>

TEST(StackTest, BasicTest) {
  ts_stl::Stack<int> s;

  ASSERT_EQ(s.Size(), 0);
  ASSERT_TRUE(s.Empty());

  for (int i = 0; i < 10000; ++i) {
    s.Push(i);
    ASSERT_EQ(s.Size(), i + 1);
    ASSERT_FALSE(s.Empty());
  }

  for (int i = 9999; i >= 0; --i) {
    ASSERT_EQ(s.Pop(), i);
    ASSERT_EQ(s.Size(), i);
  }
  ASSERT_TRUE(s.Empty());
}

TEST(StackTest, SyncTest) {
  ts_stl::Stack<int> s;

  for (int i = 0; i < 100; ++i) {
    auto f = std::async([&]() {
      for (int i = 0; i < 100; ++i) {
        s.Push(i);
      }
    });
  }

  ts_stl::Vector<std::future<int>> v;

  for (int i = 0; i < 100; ++i) {
    v.EmplaceBack(std::async([&]() {
      int sum = 0;
      for (int i = 0; i < 100; ++i) {
        sum += s.Pop();
      }
      return sum;
    }));
  }

  int sum = 0;
  for (auto &f : v) {
    sum += f.get();
  }

  ASSERT_EQ(sum, 4950 * 100);
}