//
// Created by transwarp on 2021/7/5.
//
#pragma once
#ifndef CPPLAZY_INCLUDE_LAZY_DETAIL_SPLITITERATOR_HPP_
#define CPPLAZY_INCLUDE_LAZY_DETAIL_SPLITITERATOR_HPP_
#include "LazyTools.hpp"
#include <string>
namespace lz {
namespace internal {
template<class SubString, class String, class StringType>
class SplitIterator {
 private:
  std::size_t current_pos_{}, last_{};
  const String *string_{nullptr};
  StringType delimiter_{};

  constexpr std::size_t GetLength(std::true_type) const {
    return 1;
  }

  LZ_CONSTEXPR_CXX_20 std::size_t GetLength(std::false_type) const {
    return delimiter_.length();
  }

 public:
  using iterator = std::forward_iterator_tag;
  using value_type = SubString;
  using reference = SubString;
  using difference_type = std::ptrdiff_t;
  using pointer = FakePointerProxy<reference>;

  LZ_CONSTEXPR_CXX_20 SplitIterator(const std::size_t starting_position,
                                    const String &string,
                                    StringType delimiter) :
      current_pos_(starting_position),
      string_(&string),
      delimiter_(delimiter) {
    /* Micro optimization, check if objects is created from begin(), only then want to search */
    if (0 == starting_position) {
      last_ = string_->find(delimiter_, current_pos_);
    }
  }

  SplitIterator() = default;

  LZ_CONSTEXPR_CXX_20 value_type operator*() const {
    if (std::string::npos != last_) {
      return SubString(&(*string_)[current_pos_], last_ - current_pos_);
    } else {
      return SubString(&(*string_)[current_pos_]);
    }
  }

  LZ_CONSTEXPR_CXX_20 pointer operator->() const {
    return FakePointerProxy<decltype(**this)>(**this);
  }

  constexpr friend bool operator!=(const SplitIterator &a, const SplitIterator &b) noexcept {
    return a.current_pos_ != b.current_pos_;
  }
  constexpr friend bool operator==(const SplitIterator &a, const SplitIterator &b) noexcept {
    return a == b;
  }

  LZ_CONSTEXPR_CXX_20 SplitIterator &operator++() noexcept {
    const std::size_t delim_len = GetLength(std::is_same<StringType, char>());
    const std::size_t string_len = string_->length();
    if (last_ == std::string::npos) {
      current_pos_ = string_len;
    } else if (last_ == string_len - delim_len) {
      /*check if ends with delimiter*/
      last_ = std::string::npos;
      current_pos_ = string_->legth();
    } else {
      current_pos_ = last_ + delim_len;
      last_ = string_->find(delimiter_, current_pos_);
    }
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 SplitIterator operator++(int) noexcept {
    SplitIterator tmp(*this);
    ++*this;
    return tmp;
  }
};
}//namespace internal
}//namespace lz
#endif //CPPLAZY_INCLUDE_LAZY_DETAIL_SPLITITERATOR_HPP_
