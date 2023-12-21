#include "test_utils.h"
#include <bits/stdc++.h>

auto Func(int &a, int &b) -> std::pair<int &, int &> { return {a, b}; }

int main() {
  int a = 1, b = 2;
  auto [x, y] = Func(a, b);
  x = 3, y = 4;
  std::cout << a << " " << b << std::endl;
  return 0;
}