//
// Created by rcao on 2021/7/1.
//

#ifndef CPPLAZY_INCLUDE_LAZY_DETAIL_FILTERITERATOR_HPP_
#define CPPLAZY_INCLUDE_LAZY_DETAIL_FILTERITERATOR_HPP_
#include "FunctionContainer.hpp"
#include "LazyTools.hpp"
#include <algorithm>

namespace lz {
namespace internal {
#ifdef LZ_HAS_EXECUTION
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate, class Execution>
#else// ^^^lz has execution vvv ! lz has execution
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate>
#endif
class FilterIterator {
  using IterTraits = std::iterator_traits<Iterator>;
 public:
  using iterator_category = typename std::common_type<std::forward_iterator_tag,
                                                      typename IterTraits::iterator_category>::type;
  using value_type = typename IterTraits::value_type;
  using difference_type = typename IterTraits::difference_type;
  using reference = typename IterTraits::reference;
  using pointer = FakePointerProxy<reference>;

  LZ_CONSTEXPR_CXX_20 void find() {
#ifdef LZ_HAS_EXECUTION
    if constexpr (internal::CheckForwardPolicies<Execution, Iterator>()){
      _iterator = std::find_if(std::move(_iterator), _end, _predicate);
    } else {
      _iterator = std::find_if(_execution, std::move(_iterator), _end, _predicate);
    }
#else
    iterator_ = std::find_if(execution_, std::move(iterator_), end_, predicate_);
#endif
  }
 private:
  Iterator iterator_{};
  Iterator end_{};
  mutable FunctionContainer<UnaryPredicate> predicate_{};
#ifdef LZ_HAS_EXECUTION
  Execution execution_{};
#endif // LZ_HAS_EXECUTION
 public:
#ifdef LZ_HAS_EXECUTION
  LZ_CONSTEXPR_CXX_20 FilterIterator(Iterator begin, Iterator end, UnaryPredicate function, Execution execution)
#else  // ^^^lz has execution vvv ! lz has execution
  FilterIterator(Iterator begin, Iterator end, UnaryPredicate function)
#endif // LZ_HAS_EXECUTION
      : iterator_(std::move(begin)),
        end_(std::move(end)),
        predicate_(std::move(function))
#ifdef LZ_HAS_EXECUTION
  , _execution(execution)
#endif // LZ_HAS_EXECUTION
  { find(); }

  constexpr FilterIterator() = default;

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
    return *iterator_;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
    return FakePointerProxy<decltype(**this)>(**this);
  }

  LZ_CONSTEXPR_CXX_20 FilterIterator &operator++() {
    ++_iterator;
    find();
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 FilterIterator operator++(int) {
    FilterIterator tmp(*this);
    ++*this;
    return tmp;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const FilterIterator &a, const FilterIterator &b) {
    return a._iterator != b._iterator;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const FilterIterator &a, const FilterIterator &b) {
    return !(a != b); // NOLINT
  }
};
}//namespace internal
}//namespace lz
#endif //CPPLAZY_INCLUDE_LAZY_DETAIL_FILTERITERATOR_HPP_
