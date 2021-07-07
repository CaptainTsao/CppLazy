//
// Created by transwarp on 2021/7/6.
//

#ifndef CPPLAZY_INCLUDE_LAZY_FLATTEN_HPP_
#define CPPLAZY_INCLUDE_LAZY_FLATTEN_HPP_

#include "detail/BasicIteratorView.hpp"
#include "detail/FlattenIterator.hpp"

namespace lz {
template<class Iterator, int Dims>
class Flatten final :
    public internal::BasicIteratorView<internal::FlattenIterator<Iterator, Dims>> {
 public:
  using iterator = internal::FlattenIterator<Iterator, Dims>;
  using const_iterator = iterator;
  using value_type = typename internal::FlattenIterator<Iterator, 0>::value_type;
 private:
  using Base = internal::BasicIteratorView<iterator>;
 public:
  constexpr Flatten() = default;
  LZ_CONSTEXPR_CXX_20 Flatten(Iterator begin, Iterator end) :
      Base(iterator(begin, begin, end),
           iterator(end, begin, end)) {}
};

template<LZ_CONCEPT_ITERATOR Iterator,
    int Dims = internal::CountDims<std::iterator_traits<Iterator>>::value - 1>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Flatten<Iterator, Dims> flatten_range(Iterator begin, Iterator end) {
  static_assert(std::is_default_constructible<Iterator>::value,
                "underlying iterator needs to be default constructible");
  return Flatten<Iterator, Dims>(std::move(begin), std::move(end));
}

template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>,
    int Dims = internal::CountDims<std::iterator_traits<Iterator>>::value - 1>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Flatten<Iterator, Dims> flatten(Iterable &&iterable) {
  return flatten_range<Iterator, Dims>(internal::begin(std::forward<Iterable>(iterable)),
                                       internal::end(std::forward<Iterable>(iterable)));
}

template<class, class =void>
struct Dimensions;

template<LZ_CONCEPT_ITERABLE Iterable>
struct Dimensions<Iterable, internal::EnableIF<std::is_array<Iterable>::value>>
    : internal::CountDims<internal::IterTypeFromIterable<Iterable>> {
};

template<LZ_CONCEPT_ITERABLE Iterable>
struct Dimensions<Iterable, internal::EnableIf<std::is_array<Iterable>::value>>
    : std::integral_constant<int, static_cast<int>(std::rank<Iterable>::value)> {
};
#ifdef LZ_HAS_CXX_17
template<LZ_CONCEPT_ITERABLE Iterable>
inline constexpr int DimensionsV = Dimensions<Iterable>::value;
#endif
}
#endif //CPPLAZY_INCLUDE_LAZY_FLATTEN_HPP_
