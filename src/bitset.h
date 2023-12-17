#ifndef TS_STL_BITSET_H_
#define TS_STL_BITSET_H_

#include "src/utils.h"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <string>

namespace ts_stl {

template <size_t S> class Bitset {
  static_assert(S > 0, "Bitset size must be greater than 0.");

private:
  static constexpr const size_t Num = (S + 63) >> 6;
  uint64_t bits_[Num];

public:
  Bitset() { Fill(bits_ + 0, bits_ + Num, 0); }

  Bitset(uint64_t value) {
    bits_[0] = value;
    Fill(bits_ + 1, bits_ + Num, 0);
  }

  Bitset(const Bitset<S> &other) {
    Copy(bits_, other.bits_, other.bits_ + Num);
  }

  auto operator=(const Bitset<S> &other) {
    Copy(bits_, other.bits_, other.bits_ + Num);
    return *this;
  }

  void Set() { Fill(bits_, bits_ + Num, UINT64_MAX); }

  void Set(size_t index) {
    Assert(index < S, "Bitset::Set(): index out of range.");
    bits_[index >> 6] |= static_cast<uint64_t>(1) << (index & 63);
  }

  void Reset() { Fill(bits_, bits_ + Num, 0); }

  void Reset(size_t index) {
    Assert(index < S, "Bitset::Reset(): index out of range.");
    bits_[index >> 6] &= ~(static_cast<uint64_t>(1) << (index & 63));
  }

  void Flip() {
    for (size_t i = 0; i < Num; ++i) {
      bits_[i] = ~bits_[i];
    }
  }

  void Flip(size_t index) {
    Assert(index < S, "Bitset::Flip(): index out of range.");
    bits_[index >> 6] ^= static_cast<uint64_t>(1) << (index & 63);
  }

  static constexpr size_t Size() { return S; }

  auto Test(size_t index) const -> bool {
    Assert(index < S, "Bitset::Test(): index out of range.");
    return (bits_[index >> 6] & (static_cast<uint64_t>(1) << (index & 63))) !=
           0;
  }

  class Reference {
  private:
    Bitset *bitset_;
    size_t index_;

  public:
    Reference(Bitset *bitset, size_t index) : bitset_(bitset), index_(index) {}
    Reference(const Reference &) = delete;

    auto operator=(bool b) -> Reference & {
      if (b) {
        bitset_->Set(index_);
      } else {
        bitset_->Reset(index_);
      }
      return *this;
    }

    auto operator=(const Reference &other) -> Reference & {
      if (other) {
        bitset_->Set(index_);
      } else {
        bitset_->Reset(index_);
      }
      return *this;
    }

    operator bool() const { return bitset_->Test(index_); }
  };

  auto operator[](size_t index) const -> bool { return Test(index); }

  auto operator[](size_t index) -> Reference { return Reference(this, index); }

  auto operator&(const Bitset<S> &other) const -> Bitset<S> {
    Bitset<S> result;
    for (size_t i = 0; i < Num; ++i) {
      result.bits_[i] = bits_[i] & other.bits_[i];
    }
    return result;
  }

  auto operator|(const Bitset<S> &other) const -> Bitset<S> {
    Bitset<S> result;
    for (size_t i = 0; i < Num; ++i) {
      result.bits_[i] = bits_[i] | other.bits_[i];
    }
    return result;
  }

  auto operator^(const Bitset<S> &other) const -> Bitset<S> {
    Bitset<S> result;
    for (size_t i = 0; i < Num; ++i) {
      result.bits_[i] = bits_[i] ^ other.bits_[i];
    }
    return result;
  }

  auto operator&=(const Bitset<S> &other) -> Bitset<S> & {
    for (size_t i = 0; i < Num; ++i) {
      bits_[i] &= other.bits_[i];
    }
    return *this;
  }

  auto operator|=(const Bitset<S> &other) -> Bitset<S> & {
    for (size_t i = 0; i < Num; ++i) {
      bits_[i] |= other.bits_[i];
    }
    return *this;
  }

  auto operator^=(const Bitset<S> &other) -> Bitset<S> & {
    for (size_t i = 0; i < Num; ++i) {
      bits_[i] ^= other.bits_[i];
    }
    return *this;
  }

  auto operator~() const -> Bitset<S> {
    Bitset<S> result;
    for (size_t i = 0; i < Num; ++i) {
      result.bits_[i] = ~bits_[i];
    }
    return result;
  }

  auto operator<<(size_t n) const -> Bitset<S> {
    Bitset<S> result;
    const size_t b = n >> 6, r = n & 63;
    for (size_t i = Num - 1;; --i) {
      if (i > b && r != 0) {
        result.bits_[i] = (bits_[i - b] << r) | (bits_[i - b - 1] >> (64 - r));
      } else if (i >= b) {
        result.bits_[i] = bits_[i - b] << r;
      }
      if (i <= b) {
        break;
      }
    }
    if (S & 63) {
      result.bits_[Num - 1] &= (static_cast<uint64_t>(1) << (S & 63)) - 1;
    }
    return result;
  }

  auto operator>>(size_t n) const -> Bitset<S> {
    Bitset<S> result;
    const size_t b = n >> 6, r = n & 63;
    for (size_t i = 0;; ++i) {
      if (i + b + 1 < Num && r != 0) {
        result.bits_[i] = (bits_[i + b + 1] << (64 - r)) | (bits_[i + b] >> r);
      } else if (i + b + 1 <= Num) {
        result.bits_[i] = bits_[i + b] >> r;
      }
      if (i + b + 1 >= Num) {
        break;
      }
    }
    return result;
  }

  auto operator<<=(size_t n) -> Bitset<S> & {
    const size_t b = n >> 6, r = n & 63;
    for (size_t i = Num - 1;; --i) {
      if (i > b && r != 0) {
        bits_[i] = (bits_[i - b] << r) | (bits_[i - b - 1] >> (64 - r));
      } else if (i >= b) {
        bits_[i] = bits_[i - b] << r;
      } else {
        bits_[i] = 0;
      }
      if (i == 0) {
        break;
      }
    }
    if (S & 63) {
      bits_[Num - 1] &= (static_cast<uint64_t>(1) << (S & 63)) - 1;
    }
    return *this;
  }

  auto operator>>=(size_t n) -> Bitset<S> & {
    const size_t b = n >> 6, r = n & 63;
    for (size_t i = 0; i < Num; ++i) {
      if (i + b + 1 < Num && r != 0) {
        bits_[i] = (bits_[i + b + 1] << (64 - r)) | (bits_[i + b] >> r);
      } else if (i + b + 1 <= Num) {
        bits_[i] = bits_[i + b] >> r;
      } else {
        bits_[i] = 0;
      }
    }
    return *this;
  }

  auto ToString() const -> std::string {
    std::string result(S, '0');
    for (size_t i = 0; i < S; ++i) {
      if (Test(i)) {
        result[S - i - 1] = '1';
      }
    }
    return result;
  }

  auto Count() const -> size_t {
    size_t result = 0;
    for (size_t i = 0; i < Num; ++i) {
      // result += __builtin_popcountll(bits_[i]);
      result += std::__popcount(bits_[i]);
    }
    return result;
  }

  auto All() const -> bool {
    if (uint64_t x =
            (S & 63) ? (static_cast<uint64_t>(1) << (S & 63)) - 1 : UINT64_MAX;
        bits_[0] != x) {
      return false;
    }
    return AllOf(bits_ + 1, bits_ + Num,
                 [](uint64_t x) { return x == UINT64_MAX; });
  }

  auto Any() const -> bool {
    return AnyOf(bits_, bits_ + Num, [](uint64_t x) { return x != 0; });
  }

  auto None() const -> bool {
    return NoneOf(bits_, bits_ + Num, [](uint64_t x) { return x != 0; });
  }
};
} // namespace ts_stl

#endif