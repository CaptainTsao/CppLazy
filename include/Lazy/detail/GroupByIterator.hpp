//
// Created by CaoRui on 2021/12/8.
//

#ifndef LAZY_DETAIL_GROUPBYITERATOR_HPP_
#define LAZY_DETAIL_GROUPBYITERATOR_HPP_
#include "BasicIteratorView.hpp"
#include "FunctionContainer.hpp"
#include "LazyTools.hpp"

#include <algorithm>

namespace lz {
namespace internal {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class Comparer, class Execution>
#else
template<class Iterator, class Comparer>
#endif // end LZ_HAS_EXECUTION
class GroupByIterator {
  Iterator sub_range_begin_{};
  Iterator sub_range_end_{};
  Iterator end_{};
  mutable FunctionContainer<Comparer> comparer_{};
#ifdef LZ_HAS_EXECUTION
  LZ_NO_UNIQUE_ADDRESS
  Execution execution_{};
#endif // end LZ_HAS_EXECUTION
  using IterValueType = ValueType<Iterator>;
  using Ref = RefType<Iterator>;

  LZ_CONSTEXPR_CXX_20 void advance() {
    if (sub_range_end_ == end_) {
      return;
    }
    Ref next = *sub_range_end_;
    ++sub_range_end_;
#ifdef LZ_HAS_EXECUTION
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
      sub_range_end_ = std::find_if(std::move(sub_range_end_), end_,
                                    [this, &next](const IterValueType& v) {return !compare_(v, next); });
    } else {
      sub_range_end_ = std::find_if(execution_, std::move(sub_range_end_), end_,
                                  [this, &next](const IterValueType& v) {return !compare_(v, next); });
    }
#else
    sub_range_end_ = std::find_if(std::move(sub_range_end_),
                                  end_,
                                  [this, &next](const IterValueType &v) { return !comparer_(v, next); });
#endif // end LZ_HAS_EXECUTION
  }
 public:
  using iterator_category = typename std::common_type<std::forward_iterator_tag,
                                                      typename std::iterator_traits<Iterator>::iterator_category>::type;
  using value_type = std::pair<Decay<Ref>, BasicIteratorView<Iterator>>;
  using reference = std::pair<Ref, BasicIteratorView<Iterator>>;
  using pointer = FakePointerProxy<reference>;
  using difference_type = std::ptrdiff_t;

  constexpr GroupByIterator() = default;

#ifdef LZ_HAS_EXECUTION
  LZ_CONSTEXPR_CXX_20 GroupByIterator(Iterator begin, Iterator end, Comparer comparer, Execution execution) :
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

  GroupByIterator(Iterator begin, Iterator end, Comparer comparer) :
#endif // end LZ_HAS_EXECUTION
      sub_range_end_(begin),
      sub_range_begin_(std::move(begin)),
      end_(std::move(end)),
      comparer_(std::move(comparer))
#ifdef LZ_HAS_EXECUTION
  ,
  execution_(execution)
#endif // end LZ_HAS_EXECUTION
  {
    if (sub_range_begin_ == end_) {
      return;
    }
    advance();
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
    return {*sub_range_begin_, {sub_range_begin_, sub_range_end_}};
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
    return FakePointerProxy<decltype(**this)>(**this);
  }

  LZ_CONSTEXPR_CXX_20 GroupByIterator &operator++() {
    sub_range_begin_ = sub_range_end_;
    advance();
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 GroupByIterator operator++(int) {
    GroupByIterator tmp(*this);
    ++*this;
    return tmp;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const GroupByIterator &lhs,
                                                          const GroupByIterator &rhs) noexcept {
    return lhs.sub_range_begin_ != rhs.sub_range_begin_;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const GroupByIterator &lhs,
                                                          const GroupByIterator &rhs) noexcept {
    return lhs.sub_range_begin_ == rhs.sub_range_begin_;
  }
};
}
}
#endif //LAZY_DETAIL_GROUPBYITERATOR_HPP_
