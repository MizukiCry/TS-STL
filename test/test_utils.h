#include <chrono>
#include <random>

namespace {} // namespace

template <typename F> auto function_ms(F func) -> size_t {
  auto t1 = std::chrono::high_resolution_clock::now();
  func();
  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
}

template <typename F> auto function_us(F func) -> size_t {
  auto t1 = std::chrono::high_resolution_clock::now();
  func();
  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
}

#define test_function_ms(func)                                                 \
  do {                                                                         \
    auto t = function_ms(func);                                                \
    std::cout << #func << ": " << t << " ms" << std::endl;                     \
  } while (false)

#define test_function_us(func)                                                 \
  do {                                                                         \
    auto t = function_us(func);                                                \
    std::cout << #func << ": " << t << " us" << std::endl;                     \
  } while (false)

#define test_function_ms_name(func, name)                                      \
  do {                                                                         \
    auto t = function_ms(func);                                                \
    std::cout << (name) << ": " << t << " ms" << std::endl;                    \
  } while (false)

#define test_function_us_name(func, name)                                      \
  do {                                                                         \
    auto t = function_us(func);                                                \
    std::cout << (name) << ": " << t << " us" << std::endl;                    \
  } while (false)

inline auto Random() -> size_t {
  static std::mt19937_64 Rs(
      std::chrono::system_clock::now().time_since_epoch().count());
  return Rs();
}

inline auto Random(size_t min, size_t max) -> size_t {
  return min + (Random() % (max - min + 1));
}

inline auto FastRandom() -> size_t {
  static size_t x = 131;
  return x *= 13331;
}

inline auto FastRandom(size_t min, size_t max) -> size_t {
  return min + (FastRandom() % (max - min + 1));
}

inline auto RandomString(size_t len) -> std::string {
  std::string s;
  s.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    s.push_back(static_cast<char>(Random(0, 255)));
  }
  return s;
}

inline auto TimestampMs() -> size_t {
  auto t = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             t.time_since_epoch())
      .count();
}

inline auto TimestampUs() -> size_t {
  auto t = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(
             t.time_since_epoch())
      .count();
}

template <typename Fn> auto TestTimeMs(Fn fn) -> size_t {
  auto t = TimestampMs();
  fn();
  return TimestampMs() - t;
}

template <typename Fn> auto TestTimeUs(Fn fn) -> size_t {
  auto t = TimestampUs();
  fn();
  return TimestampUs() - t;
}