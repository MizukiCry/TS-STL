#include "src/bitset.h"
#include "test_utils.h"
#include <bitset>
#include <future>
#include <gtest/gtest.h>

TEST(BitsetTest, RandomTest) {
  static constexpr const int S = 5000, T = 1000;
  std::bitset<S> b1;
  ts_stl::Bitset<S> b2;

  ASSERT_EQ(b1.to_string(), b2.ToString());

  auto Shuffle = [&b1, &b2]() {
    for (int i = 0; i < T; ++i) {
      size_t x = Random(0, S - 1);
      b1.set(x);
      b2.Set(x);
    }
    for (int i = 0; i < T; ++i) {
      size_t x = Random(0, S - 1);
      b1.reset(x);
      b2.Reset(x);
    }
    for (int i = 0; i < T; ++i) {
      size_t x = Random(0, S - 1);
      b1.flip(x);
      b2.Flip(x);
    }
  };

  for (int i = 0; i < T; ++i) {
    Shuffle();
    ASSERT_EQ(b1.to_string(), b2.ToString());
  }

  for (int i = 0; i < T; ++i) {
    Shuffle();
    b1 = ~b1;
    b2 = ~b2;
    ASSERT_EQ(b1.to_string(), b2.ToString());
  }

  for (int i = 0; i < T; ++i) {
    Shuffle();
    size_t x = Random(0, S);
    b1 <<= x;
    b2 <<= x;
    ASSERT_EQ(b1.to_string(), b2.ToString());
  }

  for (int i = 0; i < T; ++i) {
    Shuffle();
    size_t x = Random(0, S);
    b1 >>= x;
    b2 >>= x;
    ASSERT_EQ(b1.to_string(), b2.ToString());
  }
}