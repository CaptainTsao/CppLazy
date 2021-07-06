//
// Created by transwarp on 2021/7/5.
//

#ifndef CPPLAZY_INCLUDE_LAZY_DETAIL_UNIQUEITERATOR_HPP_
#define CPPLAZY_INCLUDE_LAZY_DETAIL_UNIQUEITERATOR_HPP_
#include "LazyTools.hpp"
#include "FunctionContainer.hpp"

#include <algorithm>
namespace lz {
namespace internal {

#ifdef LZ_HAS_EXECUTION
template<class Execution, LZ_CONCEPT_ITERATOR Iterator, class Compare>
#else
template<LZ_CONCEPT_ITERATOR Iterator, class Compare>
#endif// LZ_HAS_EXECUTION
class UniqueIterator {
  using IterTraits = std::iterator_traits<Iterator>;

  Iterator iterator_{};
  Iterator end_{};
  mutable FunctionContainer<Compare> compare_{};
#ifdef LZ_HAS_EXECUTION
  Execution execution_;
#endif
 public:
  using iterator = std::forward_iterator_tag;
  using value_type = typename IterTraits::value_type;
  using difference_typereference = typename IterTraits::difference_type;
  using reference = typename IterTraits::reference;
  using pointer = FakePointerProxy<reference>;

#ifdef LZ_HAS_EXECUTION
  constexpr UniqueIterator(Iterator begin, Iterator end, Compare compare, Execution execution)
#else  // ^^^ lz has execution vvv ! lz has execution
  constexpr UniqueIterator(Iterator begin, Iterator end, Compare compare)
#endif // LZ_HAS_EXECUTION
      : iterator_(std::move(begin)),
        end_(std::move(end)),
        compare_(std::move(compare))
#ifdef LZ_HAS_EXECUTION
  , execution_(execution)
#endif
  {
  }

  constexpr UniqueIterator() = default;

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
    return *iterator_;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
    return FakePointerProxy<decltype(**this)>(**this);
  }

  LZ_CONSTEXPR_CXX_20 UniqueIterator &operator++() {
#ifdef LZ_HAS_EXECUTION
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            _iterator = std::adjacent_find(std::move(_iterator), _end, _compare);
        }
        else {
            _iterator = std::adjacent_find(_execution, std::move(_iterator), _end, _compare);
        }
#else  // ^^^ lz has execution vvv ! lz has execution
    _iterator = std::adjacent_find(std::move(_iterator), _end, _compare);
#endif // LZ_HAS_EXECUTION

    if (_iterator != _end) {
      ++_iterator;
    }
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 UniqueIterator operator++(int) {
    UniqueIterator tmp(*this);
    ++*this;
    return tmp;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const UniqueIterator &a, const UniqueIterator &b) {
    return a._iterator != b._iterator;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const UniqueIterator &a, const UniqueIterator &b) {
    return !(a != b); // NOLINT
  }
};
}//namespace internal
}//namespace lz
#endif //CPPLAZY_INCLUDE_LAZY_DETAIL_UNIQUEITERATOR_HPP_
