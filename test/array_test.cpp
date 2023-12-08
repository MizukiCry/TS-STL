#include "src/array.h"
#include "test_utils.h"
#include <gtest/gtest.h>

TEST(ArrayTest, BasicTest) {
    ts_stl::FixedArray<int, 5> a, b(a);
}