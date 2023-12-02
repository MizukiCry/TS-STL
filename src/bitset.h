#ifndef TS_STL_BITSET_H_
#define TS_STL_BITSET_H_

#include "utils.h"
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
  uint64_t _bits[Num];

public:
  Bitset() { Fill(_bits + 0, _bits + Num, 0); }

  Bitset(uint64_t value) {
    _bits[0] = value;
    Fill(_bits + 1, _bits + Num, 0);
  }

  Bitset(const Bitset<S> &other) {
    Copy(_bits, other._bits, other._bits + Num);
  }

  auto operator=(const Bitset<S> &other) {
    Copy(_bits, other._bits, other._bits + Num);
    return *this;
  }

  void Set() { Fill(_bits, _bits + Num, UINT64_MAX); }

  void Set(size_t index) {
    Assert(index < S, "Bitset::Set(): index out of range.");
    _bits[index >> 6] |= static_cast<uint64_t>(1) << (index & 63);
  }

  void Reset() { Fill(_bits, _bits + Num, 0); }

  void Reset(size_t index) {
    Assert(index < S, "Bitset::Reset(): index out of range.");
    _bits[index >> 6] &= ~(static_cast<uint64_t>(1) << (index & 63));
  }

  void Flip() {
    for (size_t i = 0; i < Num; ++i) {
      _bits[i] = ~_bits[i];
    }
  }

  void Flip(size_t index) {
    Assert(index < S, "Bitset::Flip(): index out of range.");
    _bits[index >> 6] ^= static_cast<uint64_t>(1) << (index & 63);
  }

  static constexpr size_t Size() { return S; }

  auto Test(size_t index) const -> bool {
    Assert(index < S, "Bitset::Test(): index out of range.");
    return (_bits[index >> 6] & (static_cast<uint64_t>(1) << (index & 63))) !=
           0;
  }

  class Reference {
  private:
    Bitset *_bitset;
    size_t _index;

  public:
    Reference(Bitset *bitset, size_t index) : _bitset(bitset), _index(index) {}
    Reference(const Reference &) = delete;

    auto operator=(bool b) -> Reference & {
      if (b) {
        _bitset->Set(_index);
      } else {
        _bitset->Reset(_index);
      }
      return *this;
    }

    auto operator=(const Reference &other) -> Reference & {
      if (other) {
        _bitset->Set(_index);
      } else {
        _bitset->Reset(_index);
      }
      return *this;
    }

    operator bool() const { return _bitset->Test(_index); }
  };

  auto operator[](size_t index) const -> bool { return Test(index); }

  auto operator[](size_t index) -> Reference { return Reference(this, index); }

  auto operator&(const Bitset<S> &other) const -> Bitset<S> {
    Bitset<S> result;
    for (size_t i = 0; i < Num; ++i) {
      result._bits[i] = _bits[i] & other._bits[i];
    }
    return result;
  }

  auto operator|(const Bitset<S> &other) const -> Bitset<S> {
    Bitset<S> result;
    for (size_t i = 0; i < Num; ++i) {
      result._bits[i] = _bits[i] | other._bits[i];
    }
    return result;
  }

  auto operator^(const Bitset<S> &other) const -> Bitset<S> {
    Bitset<S> result;
    for (size_t i = 0; i < Num; ++i) {
      result._bits[i] = _bits[i] ^ other._bits[i];
    }
    return result;
  }

  auto operator&=(const Bitset<S> &other) -> Bitset<S> & {
    for (size_t i = 0; i < Num; ++i) {
      _bits[i] &= other._bits[i];
    }
    return *this;
  }

  auto operator|=(const Bitset<S> &other) -> Bitset<S> & {
    for (size_t i = 0; i < Num; ++i) {
      _bits[i] |= other._bits[i];
    }
    return *this;
  }

  auto operator^=(const Bitset<S> &other) -> Bitset<S> & {
    for (size_t i = 0; i < Num; ++i) {
      _bits[i] ^= other._bits[i];
    }
    return *this;
  }

  auto operator~() const -> Bitset<S> {
    Bitset<S> result;
    for (size_t i = 0; i < Num; ++i) {
      result._bits[i] = ~_bits[i];
    }
    return result;
  }

  auto operator<<(size_t n) const -> Bitset<S> {
    Bitset<S> result;
    const size_t b = n >> 6, r = n & 63;
    for (size_t i = Num - 1;; --i) {
      if (i > b && r != 0) {
        result._bits[i] = (_bits[i - b] << r) | (_bits[i - b - 1] >> (64 - r));
      } else if (i >= b) {
        result._bits[i] = _bits[i - b] << r;
      }
      if (i <= b) {
        break;
      }
    }
    result._bits[Num - 1] &= (static_cast<uint64_t>(1) << (S & 63)) - 1;
    return result;
  }

  auto operator>>(size_t n) const -> Bitset<S> {
    Bitset<S> result;
    const size_t b = n >> 6, r = n & 63;
    for (size_t i = 0;; ++i) {
      if (i + b + 1 < Num && r != 0) {
        result._bits[i] = (_bits[i + b + 1] << (64 - r)) | (_bits[i + b] >> r);
      } else if (i + b + 1 <= Num) {
        result._bits[i] = _bits[i + b] >> r;
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
        _bits[i] = (_bits[i - b] << r) | (_bits[i - b - 1] >> (64 - r));
      } else if (i >= b) {
        _bits[i] = _bits[i - b] << r;
      } else {
        _bits[i] = 0;
      }
      if (i == 0) {
        break;
      }
    }
    _bits[Num - 1] &= (static_cast<uint64_t>(1) << (S & 63)) - 1;
    return *this;
  }

  auto operator>>=(size_t n) -> Bitset<S> & {
    const size_t b = n >> 6, r = n & 63;
    for (size_t i = 0; i < Num; ++i) {
      if (i + b + 1 < Num && r != 0) {
        _bits[i] = (_bits[i + b + 1] << (64 - r)) | (_bits[i + b] >> r);
      } else if (i + b + 1 <= Num) {
        _bits[i] = _bits[i + b] >> r;
      } else {
        _bits[i] = 0;
      }
    }
    return *this;
  }

  auto ToString() -> std::string {
    std::string result(S, '0');
    for (size_t i = 0; i < S; ++i) {
      if (Test(i)) {
        result[S - i - 1] = '1';
      }
    }
    return result;
  }
};
} // namespace ts_stl

#endif