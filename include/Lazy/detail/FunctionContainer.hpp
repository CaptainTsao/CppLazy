//
// Created by rcao on 2021/6/30.
//

#ifndef CPPLAZY_FUNCTIONCONTAINER_HPP
#define CPPLAZY_FUNCTIONCONTAINER_HPP

#include "LazyTools.hpp"
#include <utility>

namespace lz {
namespace internal {
template<class>
struct AlwaysFalse : std::false_type {
};

template<class Func>
class FunctionContainer {
  mutable Func func_;
  bool is_constructed_{false};

  constexpr explicit FunctionContainer(std::false_type) {
    static_assert(AlwaysFalse<Func>::value,
                  "Please use std::function instead of a lambda in this case, because "
                  "lambda's are not default constructable pre c++20");
  }

  constexpr explicit FunctionContainer(std::true_type /* isDefaultConstructable*/) :
      func_(), is_constructed_(true) {}

  template<class F>
  LZ_CONSTEXPR_CXX_20 void construct(F &&f) {
    ::new(static_cast<void *>(std::addressof(func_))) Func(static_cast<F &&>(f));
    is_constructed_ = true;
  }

  constexpr void move(Func &&func, std::true_type) {
    func_ = std::move(func);
  }

  LZ_CONSTEXPR_CXX_20 void move(Func &&func, std::false_type) {
    reset();
    construct(std::move(func));
  }

  constexpr void reset() {
    if (is_constructed_) {
      func_.~Func();
      is_constructed_ = false;
    }
  }

  constexpr void copy(const Func &&func, std::true_type) {
    func_ = func;
  }

  LZ_CONSTEXPR_CXX_20 void copy(const Func &f, std::false_type /*isCopyAssignable*/) {
    reset();
    construct(f);
  }

 public:
  constexpr explicit FunctionContainer(const Func &&func) : func_(func), is_constructed_(true) {}

  constexpr explicit FunctionContainer(Func &&func) :
      func_(std::move(func)), is_constructed_(true) {}

  constexpr FunctionContainer() : FunctionContainer(std::is_default_constructible<Func>) {}

  LZ_CONSTEXPR_CXX_14 FunctionContainer(FunctionContainer &&other) :
      func_(std::move(other.func_)),
      is_constructed_(true) {
    other.is_constructed_ = false;
  }

  constexpr FunctionContainer(const FunctionContainer &other) : _func(other._func), _isConstructed(true) {
  }

  LZ_CONSTEXPR_CXX_20 FunctionContainer &operator=(const FunctionContainer &other) {
    if (is_constructed_ && other.is_constructed_) {
      copy(other.func_, std::is_copy_assignable<Func>());
    } else if (other.is_constructed_) {
      construct(other.func_);
    } else {
      reset();
    }
    return *this;
  }

  LZ_CONSTEXPR_CXX_20 FunctionContainer &operator=(FunctionContainer &&other) {
    if (_isConstructed && other._isConstructed) {
      move(std::move(other._func), std::is_move_assignable<Func>());
    } else if (other._isConstructed) {
      construct(std::move(other._func));
    } else if (_isConstructed) {
      reset();
    }
    return *this;
  }

  template<class... Args>
  constexpr auto operator()(Args &&...args) const -> decltype(func_(std::forward<Args>(args)...)) {
    return func_(std::forward<Args>(args)...);
  }
};
}  // namespace internal
}   // namespace lz


#endif //CPPLAZY_FUNCTIONCONTAINER_HPP
