#include "src/stack.h"
#include "src/vector.h"
#include "test_utils.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include <vector>

template <typename Fn1, typename Fn2>
void Benchmark(Fn1 fn1, Fn2 fn2, const char *name, size_t time_ms = 3000) {
  size_t times = std::max(static_cast<size_t>(10),
                          time_ms / std::max(TestTimeMs(fn1), TestTimeMs(fn2)));
  size_t t1 = 0, t2 = 0;
  for (size_t i = 0; i < times; ++i) {
    t1 += TestTimeMs(fn1);
    t2 += TestTimeMs(fn2);
  }
  std::cout << name << " benchmark: (TS-STL)" << t1 << "ms vs (STL)" << t2
            << "ms" << std::endl;
}

int main() {
  const int T1 = 1e6, T2 = 1e3;
  Benchmark(
      [] {
        ts_stl::Vector<int> v;
        for (int i = 0; i < T1; ++i)
          v.PushBack(i);
        for (int i = 0; i < T2; ++i)
          v.Insert(FastRandom(0, v.size()), i);
        for (int i = 0; i < T1; ++i)
          v.PopBack();
        v.Clear();
      },
      [] {
        std::vector<int> v;
        for (int i = 0; i < T1; ++i)
          v.push_back(i);
        for (int i = 0; i < T2; ++i)
          v.insert(v.begin() + FastRandom(0, v.size()), i);
        for (int i = 0; i < T1; ++i)
          v.pop_back();
        v.clear();
      },
      "Vector");

  Benchmark(
      [] {
        ts_stl::Stack<int> s;
        for (int i = 0; i < T1; ++i)
          s.Push(i);
        for (int i = 0; i < T1; ++i)
          s.Pop();
        for (int i = 0; i < T1; ++i)
          s.Push(i);
        s.Clear();
      },
      [] {
        std::stack<int> s;
        for (int i = 0; i < T1; ++i)
          s.push(i);
        for (int i = 0; i < T1; ++i)
          s.pop();
        for (int i = 0; i < T1; ++i)
          s.push(i);
        std::stack<int>().swap(s);
      },
      "Stack");
  return 0;
}