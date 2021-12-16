//
// Created by CaoRui on 2021/12/15.
//

#ifndef LAZY_DETAIL_CARTESIANPRODUCTITERATOR_HPP_
#define LAZY_DETAIL_CARTESIANPRODUCTITERATOR_HPP_

#include "LazyTools.hpp"
#include <numeric>

namespace lz::internal {
template<class... Iterators>
class CartesianProductIterator {
  static_assert(sizeof...(Iterators) > 1, "The size of the iterators must be greater than 1.");

 public:
  using value_type = std::tuple<ValueType<Iterators>...>;
  using reference = std::tuple<RefType<Iterators>...>;
  using pointer = FakePointerProxy<reference>;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = typename std::common_type<DiffType<Iterators>...>::type;

 private:
  std::tuple<Iterators...> begin_{};
  std::tuple<Iterators...> iterator_{};
  std::tuple<Iterators...> end_{};

#ifndef __cpp_if_constexpr
  template<std::size_t>
  constexpr EnableIf<I==0, void > next() const noexcept {  }

  template<std::size_t I>
  constexpr EnableIf<I == 0> previous() const noexcept {  }
#else
  template<std::size_t>
  LZ_CONSTEXPR_CXX_20 void next() {
    if constexpr(I == 0) {
      return;
    } else {
      auto &prev = std::get<I - 1>(iterator_);
      ++prev;

      if (prev == std::get<I - 1>(end_)) {
        if constexpr (I != 1) {
          prev = std::get<I - 1>(begin_);
          next<I - 1>();
        }
      }
    }
  }

  template<std::size_t I>
  void operator_plus_impl(const difference_type offset) {
    using lz::next;
    using std::next;

    auto &iterator = std::get<I>(iterator_);
    if constexpr(I == 0) {
      iterator_ = next(std::move(iterator_), offset);
    } else {
      using lz::next;
      using std::next;
      const auto &begin = std::get<I>(begin_);
      const auto &end = std::get<I>(end_);
      difference_type dist;
      if (offset < 0) {
        if (iterator_ == begin_) {
          iterator_ = end_;
          dist = get_iter_length(begin, iterator);
        } else {
          dist = get_iter_length(begin, iterator) + 1;
        }
      } else {
        dist = get_iter_length(iterator, end);
      }
      const auto[quot, rem] = std::lldiv(offset, dist);
      iterator = next(std::move(iterator), static_cast<difference_type>(rem));
      operator_plus_impl<I - 1>(static_cast<difference_type>(quot));
    }
  }

  template<std::size_t I>
  LZ_CONSTEXPR_CXX_20 void do_prev() {
    using Iter = Decay<decltype(std::get<I>(iterator_))>;
    if constexpr (IsBidirectional<Iter>::value) {
      --std::get<I>(iterator_);
    } else {
      const auto &beg = std::get<I>(begin_);
      const auto distance = std::distance(beg, std::get<I>(iterator_));
      std::get<I>(iterator_) = std::next(beg, std::max(difference_type{0}, distance - 1));
    }
  }

  template<std::size_t I>
  LZ_CONSTEXPR_CXX_20 void do_prev_all() {
    do_prev<I>();
    if constexpr(I > 0) {
      do_prev_all<I - 1>();
    }
  }
#endif // __cpp_if_constexpr

  template<std::size_t... Is>
  LZ_CONSTEXPR_CXX_20 reference dereference(IndexSequence<Is...>) const {
    return {*std::get<Is>(iterator_)...};
  }

  template<std::size_t... Is>
  LZ_CONSTEXPR_CXX_20 difference_type distance_impl(IndexSequence<Is...>, const CartesianProductIterator &c) const {
#if LZ_HAS_CXX_11
    const auto mul_fn = std::multiplies<difference_type>();
#else
    const auto mul_fn = std::multiplies<>();
#endif // LZ_HAS_CXX_11
    const difference_type distances[] = {static_cast<difference_type>(
                                             get_iter_length(std::get<Is>(iterator_), std::get<Is>(c.iterator_))
                                         )...};
    return std::accumulate(std::begin(distances), std::end(distances), difference_type{1}, mul_fn)
  }

};
}

#endif //LAZY_DETAIL_CARTESIANPRODUCTITERATOR_HPP_
