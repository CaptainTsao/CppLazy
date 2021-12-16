//
// Created by CaoRui on 2021/12/14.
//

#ifndef LAZY_DETAIL_RANDOMITERATOR_HPP_
#define LAZY_DETAIL_RANDOMITERATOR_HPP_

#include "LazyTools.hpp"

namespace lz::internal {
template<LZ_CONCEPT_ARITHMETIC Arithmetic, class Distribution, class Generator>
class RandomIterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = Arithmetic;
  using difference_type = std::ptrdiff_t;
  using pointer = FakePointerProxy<Arithmetic>;
  using reference = value_type;
  using result_type = value_type;
 private:
  mutable Distribution distribution_{};
  std::ptrdiff_t current_{};
  bool is_while_true_loop_{};
  Generator *generator_{nullptr};

 public:
  RandomIterator(const Distribution &distribution, Generator &generator,
                 const std::ptrdiff_t current, const bool is_while_true_loop) :
      distribution_(distribution),
      generator_(generator),
      current_(current),
      is_while_true_loop_(is_while_true_loop) {}

  RandomIterator() = default;

  LZ_NODISCARD value_type operator*() const {
    return distribution_(*generator_);
  }

  LZ_NODISCARD value_type operator()() const {
    return distribution_(*generator_);
  }

  LZ_NODISCARD pointer operator->() const {
    return FakePointerProxy<decltype(**this)>(**this);
  }

  LZ_NODISCARD result_type (min)() const noexcept {
    return (distribution_->min());
  }

  LZ_NODISCARD result_type (max)() const noexcept {
    return (distribution_->max());
  }

  RandomIterator &operator--() noexcept {
    if (!is_while_true_loop_) {
      --current_;
    }
    return *this;
  }

  RandomIterator operator--(int) noexcept {
    RandomIterator tmp(*this);
    --*this;
    return tmp;
  }
  RandomIterator &operator+=(const difference_type offset) noexcept {
    if (!is_while_true_loop_) {
      current_ += offset;
    }
    return *this;
  }
  LZ_NODISCARD RandomIterator operator+(const difference_type offset) const noexcept {
    RandomIterator tmp(*this);
    tmp += offset;
    return tmp
  }

  RandomIterator &operator-=(const difference_type offset) noexcept {
    if (!is_while_true_loop_) {
      current_ -= offset;
    }
    return *this;
  }
  LZ_NODISCARD RandomIterator operator-(const difference_type offset) const noexcept {
    RandomIterator tmp(*this);
    tmp -= offset;
    return tmp
  }

  LZ_NODISCARD friend difference_type operator-(const RandomIterator &a,
                                                const RandomIterator &b) noexcept {
    LZ_ASSERT(a.is_while_true_loop_ == b.is_while_true_loop_,
              "incompatible iterator types: both must be while true or not");
    return a.current_ - b.current_;
  }

  LZ_NODISCARD value_type operator[](const difference_type offset) const noexcept {
    return *(*this + offset);
  }

  RandomIterator &operator++() noexcept {
    if (!is_while_true_loop_) {
      ++current_;
    }
    return *this;
  }

  RandomIterator &operator++(int) noexcept {
    RandomIterator tmp(*this);
    ++*this;
    return tmp;
  }

  LZ_NODISCARD friend bool operator!=(const RandomIterator &a, const RandomIterator &b) noexcept {
    LZ_ASSERT(a.is_while_true_loop_ == b.is_while_true_loop_,
              "incompatible iterator types: both must be while true or not");
    return a.current_ != b.current_;
  }

  LZ_NODISCARD friend bool operator==(const RandomIterator &a, const RandomIterator &b) noexcept {
    return !(a != b); // NOLINT
  }

  LZ_NODISCARD friend bool operator<(const RandomIterator &a, const RandomIterator &b) noexcept {
    LZ_ASSERT(a.is_while_true_loop_ == b.is_while_true_loop_,
              "incompatible iterator types: both must be while true or not");
    return a.current_ < b.current_;
  }

  LZ_NODISCARD friend bool operator>(const RandomIterator &a, const RandomIterator &b) noexcept {
    return b < a;
  }

  LZ_NODISCARD friend bool operator<=(const RandomIterator &a, const RandomIterator &b) noexcept {
    return !(b < a); // NOLINT
  }

  LZ_NODISCARD friend bool operator>=(const RandomIterator &a, const RandomIterator &b) noexcept {
    return !(a < b); // NOLINT
  }
};
}

#endif //LAZY_DETAIL_RANDOMITERATOR_HPP_
