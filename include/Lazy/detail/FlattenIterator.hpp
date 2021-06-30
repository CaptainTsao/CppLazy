//
// Created by rcao on 2021/7/1.
//

#ifndef CPPLAZY_FLATTENITERATOR_HPP
#define CPPLAZY_FLATTENITERATOR_HPP

#include "LazyTools.hpp"

namespace lz {
    namespace internal {
        template<class T, class U = void>
        struct AliasWrapperT {
            using Type = U;
        };

        template<class T, class Enable = void>
        struct HasValueType : std::false_type {
        };

        template<class T>
        struct HasValueType<T, AliasWrapperT<typename T::value_type>> : std::true_type {
        };

        template<class T, class Enable= void>
        struct HasDifferenceType : std::false_type {
        };

        template<typename T>
        struct HasDifferenceType<T, AliasWrapperT<typename T::reference>> : std::true_type {
        };

        template<class T, class Enable = void>
        struct HasPointer : std::false_type {
        };

        template<typename T>
        struct HasPointer<T, AliasWrapperT<typename T::pointer>> : std::true_type {
        };

        template<class T, class Enable = void>
        struct HasIterCat : std::false_type {
        };

        template<class T>
        struct HasIterCat<T, AliasWrapperT<typename T::iterator_category>> : std::true_type {
        };

        template<class T, class Enable = void>
        struct HasReference : std::false_type {
        };

        template<class T>
        struct HasReference<T, AliasWrapperT<typename T::reference>> : std::true_type {
        };

        template<typename T>
        struct IsIterator {
            static constexpr bool value = HasValueType<T>::value && HasDifferenceType<T>::value && HasPointer<T>::value
                                          && HasReference<T>::value && HasIterCat<T>::value;
        };

        template<typename T, typename = void>
        struct IterTraitsOrUnderlyingType {
            using Type = T;
        };

        template<class T>
        struct IterTraitsOrUnderlyingType<T, AliasWrapperT<typename T::iterator>> {
            using Type = std::iterator_traits<typename T::iterator>;
        };

        template<class T, class U = void>
        using IterTraitsOrUnderlyingTypeT = typename IterTraitsOrUnderlyingType<T, U>::Type;

        template<class T, bool IsCont>
        struct CountDimsHelper;

        template<class T>
        struct CountDimsHelper<T, true> {
            using Inner = IterTraitsOrUnderlyingTypeT<typename T::value_type>;
            static constexpr int value = 1 + CountDimsHelper<Inner, IsIterator<Inner>::value>::value;
        };

        template<class T>
        struct CountDimsHelper<T, false> : std::integral_constant<int, 0> {
        };

        template<class T>
        struct CountDims : CountDimsHelper<T, IsIterator<T>::value> {
        };

        // Improvement of https://stackoverflow.com/a/21076724/8729023
        template<typename Iterator>
        class FlattenWrapper {
            Iterator begin_{};
            Iterator current_{};
            Iterator end_{};

            using IterTraits = std::iterator_traits<Iterator>;
        public:
            using reference = typename IterTraits::reference;
            using pointer = FakePointerProxy<reference>;
            using value_type = typename IterTraits::value_type;
            using iterator_category = typename std::common_type<std::bidirectional_iterator_tag,
                    typename IterTraits::iterator_category>::type;
            using difference_type = typename IterTraits::difference_type;

            constexpr FlattenWrapper() = default;

            LZ_CONSTEXPR_CXX_20 FlattenWrapper(Iterator current, Iterator begin, Iterator end) :
                    begin_(std::move(begin)), current_(std::move(current)), end_(std::move(end)) {}

            LZ_CONSTEXPR_CXX_20 bool HasSome() const {
                return current_ != end_;
            }

            LZ_CONSTEXPR_CXX_20 bool HasPrev() const {
                return current_ != begin_;
            }

            LZ_CONSTEXPR_CXX_20 friend bool operator!=(const FlattenWrapper &a, const FlattenWrapper &b) {
                return a._current != b._current;
            }

            LZ_CONSTEXPR_CXX_20 friend bool operator==(const FlattenWrapper &a, const FlattenWrapper &b) {
                return !(a != b); // NOLINT
            }

            LZ_CONSTEXPR_CXX_20 reference operator*() const {
                return *_current;
            }

            LZ_CONSTEXPR_CXX_20 pointer operator->() const {
                return FakePointerProxy<decltype(**this)>(**this);
            }

            LZ_CONSTEXPR_CXX_20 FlattenWrapper &operator++() {
                ++_current;
                return *this;
            }

            LZ_CONSTEXPR_CXX_20 FlattenWrapper operator++(int) {
                FlattenWrapper tmp(*this);
                ++*this;
                return tmp;
            }

            LZ_CONSTEXPR_CXX_20 FlattenWrapper &operator--() {
                --_current;
                return *this;
            }

            LZ_CONSTEXPR_CXX_20 FlattenWrapper operator--(int) {
                FlattenWrapper tmp(*this);
                ++*this;
                return tmp;
            }

            LZ_CONSTEXPR_CXX_20 difference_type distance() const {
                using lz::distance;
                using std::distance;
                return distance(_current, _end);
            }

            LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const FlattenWrapper &, const FlattenWrapper &b) {
                return b.distance();
            }

            LZ_CONSTEXPR_CXX_20 FlattenWrapper operator+(const difference_type offset) const {
                using lz::next;
                using std::next;
                FlattenWrapper tmp(*this);
                tmp._current = next(std::move(tmp._current), offset);
                return tmp;
            }
        };

        template<class Iterator, int N>
        class FlattenIterator {
            using Inner = FlattenIterator<decltype(std::begin(*std::declval<Iterator>())), N - 1>;
        public:
            using reference = typename Inner::reference;
            using pointer = FakePointerProxy<reference>;
            using value_type = typename Inner::value_type;
            using iterator_category = typename std::common_type<std::bidirectional_iterator_tag, typename Inner::iterator_category>::type;
            using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        private:
            FlattenWrapper<Iterator> outer_iter_{};
            Inner inner_iter_{};

            LZ_CONSTEXPR_CXX_20 void Advance() {
                if (inner_iter_.HasSome()) {
                    return;
                }

                for (++outer_iter_; outer_iter_.HasSome(); ++outer_iter_) {
                    const auto begin = std::begin(*outer_iter_);
                    inner_iter_ = {begin, begin, std::end(*outer_iter_)};
                    if (inner_iter_.HasSome()) {
                        return;
                    }
                }
                inner_iter_ = {};
            }

        public:
            constexpr FlattenIterator() = default;

            LZ_CONSTEXPR_CXX_20 FlattenIterator(Iterator iterator, Iterator begin, Iterator end) :
                    outer_iter_(std::move(iterator), std::move(begin), std::move(end)) {
                if (outer_iter_.HasSome()) {
                    const auto beg = std::begin(*outer_iter_);
                    inner_iter_ = {beg, beg, std::end(*outer_iter_)};
                    this->Advance();
                }
            }

            LZ_CONSTEXPR_CXX_20 bool HasSome() const {
                return outer_iter_.HasSome();
            }

            LZ_CONSTEXPR_CXX_20 bool HasPrev() const {
                return outer_iter_.HasPrev() || outer_iter_.HasPrev();
            }

            LZ_CONSTEXPR_CXX_20 friend bool operator!=(const FlattenIterator &a, const FlattenIterator &b) {
                return a.outer_iter_ != b.outer_iter_ || a.inner_iter_ != b.inner_iter_;
            }

            LZ_CONSTEXPR_CXX_20 friend bool operator==(const FlattenIterator &a, const FlattenIterator &b) {
                return !(a != b); // NOLINT
            }

            LZ_CONSTEXPR_CXX_20 reference operator*() const {
                return *inner_iter_;
            }

            LZ_CONSTEXPR_CXX_20 pointer operator->() const {
                return FakePointerProxy<decltype(**this)>(**this);
            }

            LZ_CONSTEXPR_CXX_20 FlattenIterator &operator++() {
                ++inner_iter_;
                this->Advance();
                return *this;
            }
        };
    }// namespace internal
}// namespace lz

#endif //CPPLAZY_FLATTENITERATOR_HPP
