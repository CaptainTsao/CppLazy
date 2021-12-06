//
// Created by CaoRui on 2021/12/6.
//

#ifndef LAZY_DETAIL_CHUNKSITERATOR_HPP_
#define LAZY_DETAIL_CHUNKSITERATOR_HPP_

#include "BasicIteratorView.hpp"
#include <cmath>

namespace lz {
namespace internal {
template<class, bool>
class ChunksIterator;

template<class Iterator>
class ChunksIterator<Iterator, false/* isBidirectional*/> {
  using IterTraits = std::iterator_traits<Iterator>;

 public:
  using iterator_category = typename IterTraits::iterator_category;
  using value_type = BasicIteratorView<Iterator>;
  using reference = value_type;
  using pointer = FakePointerProxy<value_type>;
  using difference_type = typename IterTraits::difference_type;
 private:
  Iterator sub_range_begin_{};
  Iterator sub_range_end_{};
  Iterator end_{};
  difference_type chunk_size_{};

  LZ_CONSTEXPR_CXX_20 void next_chunk() {
    for (different_type count = 0; count < chunk_size_ && sub_range_end_ != end_; count++, ++sub_range_end_) {}
  }
 public:
  LZ_CONSTEXPR_CXX_20 ChunksIterator(Iterator iterator, Iterator end, const std::size_t chunk_size) :
      sub_range_begin_(iterator),
      sub_range_end_(std::move(iterator)),
      end_(std::move(end)),
      chunk_size_(static_cast<difference_type>(chunk_size_)) {
    if (sub_range_begin_ == end_) {
      return;
    }
    next_chunk();
  }

  constexpr ChunksIterator() = default;

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
    return {sub_range_begin_, sub_range_end_};
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
    return FakePointerProxy<decltype(**this)>(**this);
  }

  LZ_CONSTEXPR_CXX_20 ChunksIterator &operator++() {
    sub_range_begin_ = sub_range_end_;
    next_chunk();
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 ChunksIterator operator++(int) {
    ChunksIterator tmp(*this);
    ++*this;
    return tmp;
  }

  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ChunksIterator &lhs, const ChunksIterator &rhs)
  noexcept {
    LZ_ASSERT(lhs.chunk_size_ == rhs.chunk_size_, "incompatible iterators: difference chunk sizes");
    return lhs.sub_range_begin_ != rhs.sub_range_begin_;
  }


  LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ChunksIterator& lhs, const ChunksIterator& rhs) noexcept {
    return !(lhs != rhs); // NOLINT
  }
};

} // namespace internal
} // namespace lz

#endif //LAZY_DETAIL_CHUNKSITERATOR_HPP_
