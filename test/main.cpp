#include "src/bitset.h"
#include "test_utils.h"
#include <bits/stdc++.h>

int main() {
  uint64_t x = 17391921519358330121ull;
  size_t b;
  std::cin >> b;

  assert((x >> b) == 0);

  return 0;
}