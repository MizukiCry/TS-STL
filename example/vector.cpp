#include "src/vector.h"

int main() {
  ts_stl::Vector<int> v;
  for (int i = 0; i < 10; i++) {
    v.PushBack(i);
  }
  v.Insert(5, 42);
  for (int i : v) {
    std::cout << i << std::endl;
  }
  v.Clear();
  return 0;
}