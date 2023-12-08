#include "src/array.h"
#include "src/vector.h"
#include "test_utils.h"
#include <cstring>
#include <future>
#include <gtest/gtest.h>
#include <string>

TEST(ArrayTest, BasicTest) {
  {
    ts_stl::FixedArray<int, 5> a;
    ts_stl::SyncFixedArray<std::string, 5> b;
    ts_stl::Array<std::future<int>> c(5);
    ts_stl::SyncArray<ts_stl::Vector<double>> d(5);
    (void)a;
    (void)b;
    (void)c;
    (void)d;
  }

  ts_stl::FixedArray<int, 5> a;
  EXPECT_EQ(a.Size(), 5);
  EXPECT_EQ(a.begin() + a.Size(), a.end());
  memset(a.Data(), -1, a.Size() * sizeof(decltype(a)::value_type));
  for (auto x : a) {
    EXPECT_EQ(x, -1);
  }
  EXPECT_EQ(a[2], -1);
}