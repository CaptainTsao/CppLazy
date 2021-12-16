//
// Created by CaoRui on 2021/12/14.
//

#ifndef LAZY_DETAIL_LOOPITERATORS_HPP_
#define LAZY_DETAIL_LOOPITERATORS_HPP_
#include "LazyTools.hpp"

namespace lz::internal {
template<class Iterator>
class LoopIterator {
 private:
  using IterTraits = std::iterator_traits<Iterator>;

  Iterator begin_{};
  Iterator end_{};
  Iterator iterator_{};

 public:
  using reference = typename IterTraits::reference;
  using value_type = typename IterTraits::value_type;
  using pointer = FakePointerProxy<reference>;
  using difference_type = typename IterTraits::difference_type;
  using iterator_category = typename IterTraits::iterator_category;

  LZ_CONSTEXPR_CXX_20 LoopIterator(Iterator iterator, Iterator begin, Iterator end) :
      iterator_(iterator), begin_(begin), end_(end) {}

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
    return *iterator_;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
    return FakePointerProxy<decltype(**this)>(**this);
  }

  LZ_CONSTEXPR_CXX_20 LoopIterator &operator++() {
    ++iterator_;
    if (iterator_ == end_) {
      iterator_ = begin_;
    }
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 LoopIterator operator++(int) {
    LoopIterator tmp(*this);
    ++*this;
    return tmp;
  }

  LZ_CONSTEXPR_CXX_20 LoopIterator &operator--() {
    if (iterator_ == begin_) {
      iterator_ = end_;
    }
    --iterator_;
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 LoopIterator operator--(int) {
    LoopIterator tmp(*this);
    --*this;
    return tmp;
  }

  LZ_CONSTEXPR_CXX_20 LoopIterator &operator+=(const difference_type offset) {
    iterator_ += offset % (end_ - begin_);
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 LoopIterator &operator-=(difference_type offset) {
    const auto dist = end_ - begin_;
    offset %= dist;
    iterator_ -= offset == 0 ? dist : offset;
    return *this;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 LoopIterator operator+(const difference_type offset) const {
    LoopIterator tmp(*this);
    tmp += offset;
    return tmp;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 LoopIterator operator-(const difference_type offset) const {
    LoopIterator tmp(*this);
    tmp -= offset;
    return tmp;
  }

  LZ_NODISCARD constexpr friend difference_type operator-(const LoopIterator &, const LoopIterator &) noexcept {
    return (std::numeric_limits<difference_type>::max)();
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
    return *(*this + offset);
  }

  LZ_NODISCARD constexpr friend bool operator!=(const LoopIterator &, const LoopIterator &) noexcept {
    return true;
  }

  LZ_NODISCARD constexpr friend bool operator==(const LoopIterator &a, const LoopIterator &b) noexcept {
    return !(a != b); // NOLINT
  }

  LZ_NODISCARD constexpr friend bool operator<(const LoopIterator &, const LoopIterator &) noexcept {
    return true;
  }

  LZ_NODISCARD constexpr friend bool operator>(const LoopIterator &, const LoopIterator &) noexcept {
    return true;
  }

  LZ_NODISCARD constexpr friend bool operator<=(const LoopIterator &, const LoopIterator &) noexcept {
    return true;
  }

  LZ_NODISCARD constexpr friend bool operator>=(const LoopIterator &, const LoopIterator &) noexcept {
    return true;
  }
};
}

#endif //LAZY_DETAIL_LOOPITERATORS_HPP_
