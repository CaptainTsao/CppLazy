//
// Created by CaoRui on 2021/12/7.
//

#ifndef LAZY_DETAIL_RANGEITERATOR_HPP_
#define LAZY_DETAIL_RANGEITERATOR_HPP_
#include <cmath>
#include <iterator>
#include "LazyTools.hpp"
namespace lz {
namespace internal {
#ifdef __cpp_if_constexpr
template<class ValueType>
std::ptrdiff_t plus_impl(const ValueType difference, const ValueType step) noexcept(!std::is_floating_point<ValueType>) {
  if noexcept (std::is_floating_pointer_v<ValueType>) {
    return static_cast<std::ptrdiff_t>(std::ceil(difference/step));
  } else  {
    return static_cast<std::ptrdiff_t>(std::ceil(difference/step));
  }
}
#else
template<class ValueType>
EnableIf<std::is_floating_point<ValueType>::value, std::ptrdiff_t> plus_impl(const ValueType difference,
                                                                             const ValueType step) {
  return static_cast<std::ptrdiff_t>(std::ceil(difference / step));
}

template<class ValueType>
constexpr
EnableIf<!std::is_floating_point<ValueType>::value, std::ptrdiff_t>
plus_impl(const ValueType difference,
          const ValueType step) {
  return static_cast<std::ptrdiff_t>(roundeven(difference / step));
}
#endif  // __cpp_if_constexpr

template<class Arithmetic>
class RangeIterator {
 private:
  Arithmetic iterator_{};
  Arithmetic step_{};
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = Arithmetic;
  using difference_type = std::ptrdiff_t;
  using pointer = Arithmetic;
  using reference = Arithmetic;

  constexpr RangeIterator(const Arithmetic iterator, const Arithmetic step) noexcept:
      iterator_(iterator), step_(step) {}

  constexpr RangeIterator() = default;

  LZ_NODISCARD constexpr value_type operator*() const noexcept {
    return _iterator;
  }

  LZ_NODISCARD constexpr pointer operator->() const noexcept {
    return FakePointerProxy<value_type>(**this);
  }

  LZ_CONSTEXPR_CXX_14 RangeIterator &operator++() noexcept {
    _iterator += _step;
    return *this;
  }

  LZ_CONSTEXPR_CXX_14 RangeIterator operator++(int) noexcept {
    RangeIterator tmp(*this);
    ++*this;
    return tmp;
  }

  LZ_CONSTEXPR_CXX_14 RangeIterator &operator--() noexcept {
    _iterator -= _step;
    return *this;
  }

  LZ_CONSTEXPR_CXX_14 RangeIterator operator--(int) noexcept {
    RangeIterator tmp(*this);
    --*this;
    return tmp;
  }

  LZ_NODISCARD friend difference_type operator-(const RangeIterator &a, const RangeIterator &b)
  noexcept(!std::is_floating_point<Arithmetic>::value) {
    LZ_ASSERT(a.step_ == b.step_, "incompatible iterator types: difference step size");
    const auto difference = a.iterator_ - b.iterator_;
    return std::abs(plusImpl(static_cast<Arithmetic>(difference), a.step_));
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const noexcept {
    return *(*this + offset);
  }

  constexpr RangeIterator &operator+=(const difference_type value) noexcept {
    iterator_ += static_cast<Arithmetic>(value) * step_;
    return *this;
  }

  LZ_NODISCARD constexpr RangeIterator operator+(const difference_type value) const noexcept {
    RangeIterator tmp(*this);
    tmp += value;
    return tmp;
  }
  constexpr RangeIterator operator-=(const difference_type value) noexcept {
    _iterator -= static_cast<Arithmetic>(value) * _step;
    return *this;
  }

  LZ_NODISCARD constexpr RangeIterator operator-(const difference_type value) const noexcept {
    RangeIterator tmp(*this);
    tmp -= value;
    return tmp;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator!=(const RangeIterator &a, const RangeIterator &b) noexcept {
    LZ_ASSERT(a.step_ == b.step_, "incompatible iterator types: difference step size");
    return a.step_ < 0 ? a.iterator_ > b.iterator_ : a.iterator_ < b.iterator_;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator==(const RangeIterator &a, const RangeIterator &b) noexcept {
    return !(a != b); // NOLINT
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const RangeIterator &a, const RangeIterator &b) noexcept {
    return a.iterator_ < b.iterator_;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const RangeIterator &a, const RangeIterator &b) noexcept {
    return b < a; // NOLINT
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const RangeIterator &a, const RangeIterator &b) noexcept {
    return !(b < a); // NOLINT
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const RangeIterator &a, const RangeIterator &b) noexcept {
    return !(a < b); // NOLINT
  }

};
}
}

#endif //CPPLAZY_INCLUDE_LAZY_DETAIL_RANGEITERATOR_HPP_
