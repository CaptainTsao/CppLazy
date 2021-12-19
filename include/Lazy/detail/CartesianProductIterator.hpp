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
  }

  template <std::size_t I, class Iter = Decay<decltype(std::get<I>(iterator_))>>
  LZ_CONSTEXPR_CXX_20 EnableIf<IsBidirectional<Iter>::value> do_prev() {
    --std::get<I>(iterator_);
  }

  template <std::size_t I, class Iter = Decay<decltype(std::get<I>(iterator_))>>
  LZ_CONSTEXPR_CXX_20 EnableIf<!IsBidirectional<Iter>::value> do_prev() {
    using lz::next;
    using std::next;

    const auto* beg = std::get<I>(begin_);
    const auto distance = get_iter_length(begin, std::get<I>(iterator_));
    std::get<I>(iterator_) = next(beg, distance - 1);
  }

  template <std::size_t I>
  LZ_CONSTEXPR_CXX_20 EnableIf<I == 0> do_prev_all() {
    do_prev<0>();
  }

  template <std::size_t I>
  LZ_CONSTEXPR_CXX_20 EnableIf<(I > 0)> do_prev_all() {
    do_prev<I>();
    do_prev_all<I - 1>();
  }

  template <std::size_t I>
  LZ_CONSTEXPR_CXX_20 EnableIf<(I > 0)> previou() {
    if (iterator_ == end_) {
      do_prev_all<I - 1>();
    } else {
      auto& prev = std::get<I - 1>(iterator_);
      if (prev == std::get<I - 1>(begin_)) {
        prev = std::get<I - 1>(end_);
        do_prev<I - 1>();
        previou<I - 1>();
      } else {
        do_prev<I - 1>();
      }
    }
  }

#ifdef LZ_MSVC
#pragma warning(pop)
#endif  // LZ_MSVC

  template <std::size_t I>
  LZ_CONSTEXPR_CXX_20 EnableIf<I == 0> operator_plus_impl(
      const difference_type offset) {
    using lz::next;
    using std::next;
    auto& iterator = std::get<0>(iterator_);
    iterator = next(std::move(iterator), offset);
  }
  template <std::size_t I>
  LZ_CONSTEXPR_CXX_20 EnableIf<(I > 0)> operator_plus_impl(
      const difference_type offset) {
    using lz::next;
    using std::next;

    auto& iterator = std::get<I>(iterator_);
    const auto& begin = std::get<I>(begin_);
    const auto& end = std::get<I>(end_);
    difference_type dist;
    if (offset < 0) {
      if (iterator == begin) {
        iterator = end;
        dist = get_iter_length(begin, iterator);
      } else {
        dist = get_iter_length(begin, iterator) + 1;
      }
    } else {
      dist = get_iter_length(iterator, end);
    }
    const auto offsets = std::lldiv(offset, dist);
    iterator =
        next(std::move(iterator), static_cast<difference_type>(offsets.rem));
    operator_plus_impl<I - 1>(static_cast<difference_type>(offsets.quot));
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
