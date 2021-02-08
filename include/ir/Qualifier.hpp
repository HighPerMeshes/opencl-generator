#ifndef QUALIFIER_HPP
#define QUALIFIER_HPP

#include <ostream>
#include <string_view>
#include <sstream>

namespace ct
{

  struct Qualifier
  {

    std::string_view value;
  };

  bool operator==(const Qualifier &lhs, const Qualifier &rhs)
  {
    return lhs.value == rhs.value;
  }
  bool operator!=(const Qualifier &lhs, const Qualifier &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Qualifier &qualifier) { stream << qualifier.value; }

  template <typename Visitor>
  Qualifier transform(Visitor &&visitor, Qualifier &&qualifier)
  {
    return {visitor.visit(std::move(qualifier.value))};
  }

  constexpr Qualifier Global() { return {"global"}; }
  constexpr Qualifier Const() { return {"const"}; }
  constexpr Qualifier Reference() { return {"&"}; }
  constexpr Qualifier Pointer() { return {"*"}; }
  constexpr Qualifier RValue() { return {"&&"}; }
  constexpr Qualifier Kernel() { return {"kernel"}; }
  constexpr Qualifier Constant() { return {"constant"}; }

  
  const Type * get_type(const Qualifier&) {
    return nullptr;
  }

} // namespace ct

#endif /* QUALIFIER_HPP */
