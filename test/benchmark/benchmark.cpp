#include "src/deque.h"
#include "src/queue.h"
#include "src/stack.h"
#include "src/vector.h"
#include "test/test_utils.h"
#include <algorithm>
#include <deque>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
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

  (void)[] {
    // Warm Up
    std::string s;
    for (int i = 0; i < T1; ++i) {
      s.push_back(static_cast<char>(
          FastRandom(static_cast<size_t>('a'), static_cast<size_t>('z'))));
    }
    s.clear();
  }
  ();

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

  Benchmark(
      [] {
        ts_stl::Deque<int> q;
        for (int i = 0; i < T1; ++i)
          q.PushBack(i);
        for (int i = 0; i < T2; ++i)
          q.Insert(FastRandom(0, q.size()), i);
        for (int i = 0; i < T1; ++i)
          q.PopBack();
        q.Clear();
      },
      [] {
        std::deque<int> q;
        for (int i = 0; i < T1; ++i)
          q.push_back(i);
        for (int i = 0; i < T2; ++i)
          q.insert(q.begin() + FastRandom(0, q.size()), i);
        for (int i = 0; i < T1; ++i)
          q.pop_back();
        q.clear();
      },
      "Deque");

  Benchmark(
      [] {
        ts_stl::Queue<int> q;
        for (int i = 0; i < T1; ++i)
          q.Push(i);
        for (int i = 0; i < T1; ++i)
          q.Pop();
      },
      [] {
        std::queue<int> q;
        for (int i = 0; i < T1; ++i)
          q.push(i);
        for (int i = 0; i < T1; ++i)
          q.pop();
      },
      "Queue");
  return 0;
}