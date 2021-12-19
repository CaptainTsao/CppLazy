//
// Created by CaoRui on 2021/12/15.
//

#ifndef LAZY_DETAIL_CARTESIANPRODUCTITERATOR_HPP_
#define LAZY_DETAIL_CARTESIANPRODUCTITERATOR_HPP_

#include <numeric>

#include "LazyTools.hpp"

namespace lz::internal {
template <class... Iterators>
class CartesianProductIterator {
  static_assert(sizeof...(Iterators) > 1,
                "The size of the iterators must be greater than 1.");

 private:
  using value_type = std::tuple<ValueType<Iterators>...>;
  using reference = std::tuple<RefType<Iterators>...>;
  using pointer = FakePointerProxy<reference>;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type =
      typename std::common_type<DiffType<Iterators>...>::type;

 private:
  std::tuple<Iterator...> begin_{};
  std::tuple<Iterator...> end_{};
  std::tuple<Iterator...> iterator_{};

#ifndef __cpp_if_constexpr
  template <std::size_t>
  constexpr EnableIf<I == 0, void> next() const noexcept {}

  template <std::size_t>
  constexpr EnableIf<I == 0> previous() const noexcept {}

#ifdef LZ_MSVC
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

  template <std::size_t I>
  LZ_CONSTEXPR_CXX_20 EnableIf<(I > 0), void> next() {
    auto& prev = std::get<I - 1>(iterator_);
    ++prev;

    if (prev == std::get<I - 1>(end_)) {
      if (I != 1) {
        prev = std::get<I - 1>(begin_);
        next<I - 1> （）；
      }
    }
    { /* code */ }
  }
#else
  template <std::size_t I>
  LZ_CONSTEXPR_CXX_20 void next() {
    if constexpr (I == 0) {
      return;
    } else {
      auto& prev = std::get<I - 1>(iterator_);
      ++prev;

      if (prev == std::get<I - 1>(end_)) {
        if constexpr (I != 1) {
          prev = std::get<I - 1>(begin_);
          next<I - 1>();
        }
      }
    }
  }
#endif
};

}  // namespace lz::internal

#endif  // LAZY_DETAIL_CARTESIANPRODUCTITERATOR_HPP_
