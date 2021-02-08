#ifndef MATCH_HPP
#define MATCH_HPP

#include <optional>
#include <utility>

#include <clang/Tooling/Tooling.h>

template<typename T, typename Op> struct CaseImpl {

  Op op;
  CaseImpl(Op op) : op { op } {};

  template<typename Ref> 
  auto operator()(Ref& reference) -> std::optional<decltype(op(std::declval<T>()))> {

    auto * casted = llvm::dyn_cast_or_null<T>(&reference);
    if(casted) {
      return { op(*casted) };
    }
    else return {};
  } 

};

template<typename T, typename Op> auto Case(Op op) -> CaseImpl<T, Op> {
  return { op };
}

template<typename T> struct Match {

  Match(const T& reference) : reference { reference } {}
  const T& reference;


  template<typename Case> auto operator()(Case&& headCase) {
      return headCase(reference);
  }

  template<typename Case, typename... Cases> auto operator()(Case&& headCase, Cases&&... tailCases) {
    auto result = headCase(reference);
    return result ? result : (*this)(std::forward<Cases>(tailCases)...);
  }

};

#endif