#ifndef HPM_CODETRANSFORMATION_UTIL_HPP
#define HPM_CODETRANSFORMATION_UTIL_HPP

#include <clang/Tooling/Tooling.h>
#include <llvm/Support/Casting.h>

#include <util/print_with_delimiter.hpp>
#include <util/print.hpp>
#include <util/visit_all.hpp>

#include <sstream>
#include <string_view>
#include <optional>

#include <iostream>

namespace ct {

template<typename Message>
void println(Message&& message) {
  print(std::cout, std::forward<Message>(message));
}

void horizontal_line() {
  println("\n----------\n");
}

template<typename Op>
void for_args(const clang::CallExpr * e, Op op) {
  for(size_t i = 0; i < e->getNumArgs(); ++i) {
    op(
      e->getArgs()[i]
    );
  }
}

template<typename Message>
[[noreturn]] auto fail_with_message(Message&& message) {
  llvm::errs() << std::forward<Message>(message) << "\n";
  std::exit(EXIT_FAILURE);
}

template<typename StringType>
auto remove_templates(StringType&& s) {
  
  using namespace std;

  auto bracket_pos = find(begin(s), end(s), '<');

  return string { begin(s), bracket_pos };
}

template<typename... Messages>
std::string make_string(Messages&&... messages) {
  std::stringstream ss;
  auto feed_ss = [&ss](auto&& message) { ss << std::forward<decltype(message)>(message); }; 
  (
    feed_ss(std::forward<Messages>(messages)), ...
  );
  return ss.str();
}

template<typename T>
bool vector_equals(const std::vector<T>& lhs, const std::vector<T>& rhs) {
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

auto TypeAsString(const clang::QualType& t) {
  
  using namespace clang;

  PrintingPolicy policy {
    LangOptions {}
  };
  
  auto type = t;
  type.removeLocalConst();
  type.removeLocalVolatile();
  type.removeLocalRestrict();

  return type.getAsString(policy);
}

}

#endif
