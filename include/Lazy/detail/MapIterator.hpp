//
// Created by rcao on 2021/6/30.
//

#ifndef CPPLAZY_MAPITERATOR_HPP
#define CPPLAZY_MAPITERATOR_HPP
#include "LazyTools.hpp"
#include "FunctionContainer.hpp"

namespace lz {
    namespace internal {
        template<LZ_CONCEPT_ITERATOR Iterator, class Function>
        class MapIterator {
        private:
            Iterator iterator_{};
            mutable FunctionContainer<Function> function_container_{};
            using IterTraits = std::iterator_traits<Iterator>;
        public:
            using reference = decltype(function_container_(*iterator_));
            using value_type = Decay<reference>;
            using iterator_category = typename IterTraits::iterator_category;
            using difference_type = typename IterTraits::difference_type;
            using pointer = FakePointerProxy<reference>;
        };
    }// namespace internal
}// namespace lz
#endif //CPPLAZY_MAPITERATOR_HPP
