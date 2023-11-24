#include "test_utils.h"
#include <bits/stdc++.h>

int main() {
  const int T = 1e6;
  test_function_ms_name(
      [] {
        std::deque<int> q;
        for (int i = 0; i < T; ++i) {
          q.insert(q.begin() + FastRandom(0, i), i);
        }
      },
      "std::deque");
  test_function_ms_name(
      [] {
        std::vector<int> v;
        for (int i = 0; i < T; ++i) {
          v.insert(v.begin() + FastRandom(0, i), i);
        }
      },
      "std::vector");
  return 0;
}