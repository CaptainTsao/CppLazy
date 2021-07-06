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
}
#endif //CPPLAZY_INCLUDE_LAZY_FLATTEN_HPP_
