//
// Created by CaoRui on 2021/12/7.
//

#ifndef LAZY_DETAIL_CONCATENATEITERATOR_HPP_
#define LAZY_DETAIL_CONCATENATEITERATOR_HPP_

#include "LazyTools.hpp"
#include <numeric>

namespace lz {
namespace internal {
#ifndef __cpp_if_constexpr
template<class Tuple, std::size_t I, class = void>
struct PlusPlus {
  LZ_CONSTEXPR_CXX_20 void operator()(Tuple &iterators, const Tuple &end) const {
    if (std::get<I>(iterators) != std::get<I>(end)) {
      ++std::get<I>(iterators);
    } else {
      PlusPlus<Tuple, I + 1>()(iterators, end);
    }
  }
};

template<class Tuple, std::size_t I>
struct PlusPlus<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value>> {
  LZ_CONSTEXPR_CXX_20 void operator()(const Tuple & /*iterators*/, const Tuple & /*end*/) const {}
};

template<class Tuple, std::size_t, class = void>
struct NotEqual {
  LZ_CONSTEXPR_CXX_20 bool operator()(const Tuple &iterators, const Tuple &end) const noexcept {
    const bool iter_has_value = std::get<I>(iterators) != std::get<I>(end);
    return iter_has_value ? iter_has_value : NotEqual<Tuple, I + 1>()(iterators, end);
  }
};

template<class Tuple, std::size_t I>
struct NotEqual<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
  LZ_CONSTEXPR_CXX_20 bool operator()(const Tuple &iterators, const Tuple &end) const noexcept {
    return std::get<I>(iterators) != std::get<I>(end);
  }
};

template<class Tuple, std::size_t I, class = void>
struct Deref {
  LZ_CONSTEXPR_CXX_20 auto operator()(const Tuple &iterators,
                                      const Tuple &end) const -> decltype(*std::get<I>(iterators)) {
    return std::get<I>(iterators) != std::get<I>(end) ? *std::get<I>(iterators) : Deref<Tuple, I + 1>()(iterators, end);
  }
};

template<class Tuple, std::size_t I>
struct Deref<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
  LZ_CONSTEXPR_CXX_20 auto operator()(const Tuple &iterators,
                                      const Tuple &) const -> decltype(*std::get<I>(iterators)) {
    return *std::get<I>(iterators);
  }
};

template<class Tuple, std::size_t I>
struct MinusMinus {
  LZ_CONSTEXPR_CXX_20 void operator()(Tuple &iterators, const Tuple &begin, const Tuple &end) const {
    auto &current = std::get<I>(iterators);
    if (current != std::get<I>(begin)) {
      --current;
    } else {
      MinusMinus<Tuple, I - 1>()(iterators, begin, end);
    }
  }
};

template<class Tuple>
struct MinusMinus<Tuple, 0> {
  LZ_CONSTEXPR_CXX_20 void operator()(Tuple &iterators, const Tuple &, const Tuple &) const {
    --std::get<0>(iterators);
  }
};

template<class Tuple, std::size_t I>
struct MinIs {
  template<class DifferenceType>
  LZ_CONSTEXPR_CXX_20 void
  operator()(Tuple &iterators, const Tuple &begin, const Tuple &end, const DifferenceType offset) const {
    using TupElem = TupleElement<I, Tuple>;
    const TupElem current = std::get<I>(iterators);
    const TupElem current_begin = std::get<I>(begin);
    // Current is begin, move on to next iterator
    if (current == current_begin) {
      MinIs<Tuple, I - 1>()(iterators, begin, end, offset);
    } else {
      const auto dist = std::get<I>(end) - current;
      if (dist <= offset) {
        std::get<I>(iterators) = std::get<I>(begin);
        MinIs<Tuple, I - 1>()(iterators, begin, end, dist == 0 ? DifferenceType{1} : offset - dist);
      } else {
        std::get<I>(iterators) -= offset;
      }
    }
  }
};

template<class Tuple>
struct MinIs<Tuple, 0> {
  template<class DifferenceType>
  LZ_CONSTEXPR_CXX_20 void
  operator()(Tuple &iterators, const Tuple & /* begin */, const Tuple & /*end*/, const DifferenceType offset) const {
    using TupElem = TupleElement<0, Tuple>;
    TupElem &current = std::get<0>(iterators);
    current -= offset;
  }
};

template<class Tuple, std::size_t I, class = void>
struct PlusIs {
  template<class DifferenceType>
  LZ_CONSTEXPR_CXX_20 void operator()(Tuple &iterators, const Tuple &end, const DifferenceType offset) const {
    using TupElem = TupleElement<I, Tuple>;
    TupElem &current_iterator = std::get<I>(iterators);
    const TupElem current_end = std::get<I>(end);
    const auto dist = current_end - current_iterator;
    if (dist > offset) {
      current_iterator += offset;
    } else {
      // Moves to end
      currentIterator += dist;
      PlusIs<Tuple, I + 1>()(iterators, end, offset - dist);
    }
  }
};

template<class Tuple, std::size_t I>
struct PlusIs<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
  template<class DifferenceType>
  constexpr void operator()(Tuple & /*iterators*/, const Tuple & /*end*/, const DifferenceType /*offset*/) const {
  }
};
#else
template<class Tuple, std::size_t I>
struct PlusPlus {
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const Tuple& end) const {
        if constexpr (I == std::tuple_size_v<Decay<Tuple>>) {
            static_cast<void>(iterators);
            static_cast<void>(end);
            return;
        }
        else {
            if (std::get<I>(iterators) != std::get<I>(end)) {
                ++std::get<I>(iterators);
            }
            else {
                PlusPlus<Tuple, I + 1>()(iterators, end);
            }
        }
    }
};

template<class Tuple, std::size_t I>
struct NotEqual {
    LZ_CONSTEXPR_CXX_20 bool operator()(const Tuple& iterators, const Tuple& end) const {
        if constexpr (I == std::tuple_size_v<Decay<Tuple>> - 1) {
            return std::get<I>(iterators) != std::get<I>(end);
        }
        else {
            const bool iterHasValue = std::get<I>(iterators) != std::get<I>(end);
            return iterHasValue ? iterHasValue : NotEqual<Tuple, I + 1>()(iterators, end);
        }
    }
};

template<class Tuple, std::size_t I>
struct Deref {
    LZ_CONSTEXPR_CXX_20 auto operator()(const Tuple& iterators, const Tuple& end) const -> decltype(*std::get<I>(iterators)) {
        if constexpr (I == std::tuple_size_v<Decay<Tuple>> - 1) {
            static_cast<void>(end);
            return *std::get<I>(iterators);
        }
        else {
            return std::get<I>(iterators) != std::get<I>(end) ? *std::get<I>(iterators) : Deref<Tuple, I + 1>()(iterators, end);
        }
    }
};

template<class Tuple, std::size_t I>
struct MinusMinus {
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const Tuple& begin, const Tuple& end) const {
        if constexpr (I == 0) {
            static_cast<void>(begin);
            static_cast<void>(end);
            --std::get<0>(iterators);
        }
        else {
            auto& current = std::get<I>(iterators);
            if (current != std::get<I>(begin)) {
                --current;
            }
            else {
                MinusMinus<Tuple, I - 1>()(iterators, begin, end);
            }
        }
    }
};

template<class Tuple, std::size_t I>
struct MinIs {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_20 void
    operator()(Tuple& iterators, const Tuple& begin, const Tuple& end, const DifferenceType offset) const {
        using TupElem = TupleElement<I, Tuple>;

        if constexpr (I == 0) {
            static_cast<void>(begin);
            static_cast<void>(end);
            TupElem& current = std::get<0>(iterators);
            current -= offset;
        }
        else {
            const TupElem currentBegin = std::get<I>(begin);
            const TupElem current = std::get<I>(iterators);
            // Current is begin, move on to next iterator
            if (current == currentBegin) {
                MinIs<Tuple, I - 1>()(iterators, begin, end, offset);
            }
            else {
                const auto dist = std::get<I>(end) - current;
                if (dist <= offset) {
                    std::get<I>(iterators) = std::get<I>(begin);
                    MinIs<Tuple, I - 1>()(iterators, begin, end, dist == 0 ? DifferenceType{ 1 } : offset - dist);
                }
                else {
                    std::get<I>(iterators) -= offset;
                }
            }
        }
    }
};

template<class Tuple, std::size_t I>
struct PlusIs {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const Tuple& end, const DifferenceType offset) const {
        using lz::distance;
        using std::distance;
        if constexpr (I == std::tuple_size_v<Decay<Tuple>> - 1) {
            static_cast<void>(iterators);
            static_cast<void>(end);
            static_cast<void>(offset);
            return;
        }
        else {
            using TupElem = TupleElement<I, Tuple>;
            TupElem& currentIterator = std::get<I>(iterators);
            const TupElem currentEnd = std::get<I>(end);
            const auto dist = currentEnd - currentIterator;
            if (dist > offset) {
                currentIterator += offset;
            }
            else {
                // Moves to end
                currentIterator += dist;
                PlusIs<Tuple, I + 1>()(iterators, end, offset - dist);
            }
        }
    }
};
#endif // __cpp_if_constexpr

template<LZ_CONCEPT_ITERATOR... Iterators>
class ConcatenateIterator {
  using IterTuple = std::tuple<Iterators...>;
  IterTuple iterators_{};
  IterTuple begin_{};
  IterTuple end_{};

  using FirstTupleIterator = std::iterator_traits<TupleElement<0, IterTuple>>;
 public:
  using value_type = typename FirstTupleIterator::value_type;
  using difference_type = typename std::common_type<DiffType<Iterators>...>::type;
  using reference = typename FirstTupleIterator::reference;
  using pointer = FakePointerProxy<reference>;
  using iterator_category = typename std::common_type<IterCat<Iterators>...>::type;

 private:
  template<std::size_t... I>
  LZ_CONSTEXPR_CXX_20 difference_type minus(IndexSequence<I...>, const ConcatenateIterator &other) const {
    const difference_type totals[] = {static_cast<differenct_type>(std::get<I>(iterators_) -
        std::get<I>(other.iterators_))...};
    return std::accumulate(std::begin(totals), std::end(totals), difference_type{0});
  }
};

} // namespace internal
} // namespace lz
#endif //LAZY_DETAIL_CONCATENATEITERATOR_HPP_
