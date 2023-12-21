#include "src/deque.h"
#include "src/hashmap.h"
#include "src/map.h"
#include "src/queue.h"
#include "src/stack.h"
#include "src/vector.h"
#include "test/test_utils.h"
#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

const int T3 = 1e3, T5 = 1e5, T6 = 1e6, T7 = 1e7, T8 = 1e8;

template <typename Fn1, typename Fn2>
void Benchmark(Fn1 fn1, Fn2 fn2, const char *name, size_t time_ms = 3000) {
  (void)[] {
    // Warm Up
    std::string s;
    for (int i = 0; i < T6; ++i) {
      s.push_back(static_cast<char>(
          FastRandom(static_cast<size_t>('a'), static_cast<size_t>('z'))));
    }
    s.clear();
  }
  ();

  size_t times = std::max(static_cast<size_t>(3),
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
  Benchmark(
      [] {
        ts_stl::Vector<int> v;
        for (int i = 0; i < T6; ++i)
          v.PushBack(i);
        for (int i = 0; i < T3; ++i)
          v.Insert(FastRandom(0, v.size()), i);
        for (int i = 0; i < T6; ++i)
          v.PopBack();
        v.Clear();
      },
      [] {
        std::vector<int> v;
        for (int i = 0; i < T6; ++i)
          v.push_back(i);
        for (int i = 0; i < T3; ++i)
          v.insert(v.begin() + FastRandom(0, v.size()), i);
        for (int i = 0; i < T6; ++i)
          v.pop_back();
        v.clear();
      },
      "Vector");

  Benchmark(
      [] {
        ts_stl::Stack<int> s;
        for (int i = 0; i < T6; ++i)
          s.Push(i);
        for (int i = 0; i < T6; ++i)
          s.Pop();
        for (int i = 0; i < T6; ++i)
          s.Push(i);
        s.Clear();
      },
      [] {
        std::stack<int> s;
        for (int i = 0; i < T6; ++i)
          s.push(i);
        for (int i = 0; i < T6; ++i)
          s.pop();
        for (int i = 0; i < T6; ++i)
          s.push(i);
        std::stack<int>().swap(s);
      },
      "Stack");

  Benchmark(
      [] {
        ts_stl::Deque<int> q;
        for (int i = 0; i < T6; ++i)
          q.PushBack(i);
        for (int i = 0; i < T3; ++i)
          q.Insert(FastRandom(0, q.size()), i);
        for (int i = 0; i < T6; ++i)
          q.PopBack();
        q.Clear();
      },
      [] {
        std::deque<int> q;
        for (int i = 0; i < T6; ++i)
          q.push_back(i);
        for (int i = 0; i < T3; ++i)
          q.insert(q.begin() + FastRandom(0, q.size()), i);
        for (int i = 0; i < T6; ++i)
          q.pop_back();
        q.clear();
      },
      "Deque");

  Benchmark(
      [] {
        ts_stl::Queue<int> q;
        for (int i = 0; i < T6; ++i)
          q.Push(i);
        for (int i = 0; i < T6; ++i)
          q.Pop();
      },
      [] {
        std::queue<int> q;
        for (int i = 0; i < T6; ++i)
          q.push(i);
        for (int i = 0; i < T6; ++i)
          q.pop();
      },
      "Queue");

  Benchmark(
      [] {
        ts_stl::HashMap<size_t, size_t> m;
        for (int i = 0; i < T6; ++i) {
          size_t x = FastRandom(), y = FastRandom();
          m[x] = y;
        }
      },
      [] {
        std::unordered_map<size_t, size_t> m;
        for (int i = 0; i < T6; ++i) {
          size_t x = FastRandom(), y = FastRandom();
          m[x] = y;
        }
      },
      "HashMap");

  Benchmark(
      [] {
        ts_stl::Map<size_t, size_t> m;
        for (int i = 0; i < T5; ++i) {
          size_t x = FastRandom(), y = FastRandom();
          m[x] = y;
        }
      },
      [] {
        std::map<size_t, size_t> m;
        for (int i = 0; i < T5; ++i) {
          size_t x = FastRandom(), y = FastRandom();
          m[x] = y;
        }
      },
      "Map");
  return 0;
}