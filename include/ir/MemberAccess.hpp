#ifndef MEMBERACCESS_HPP
#define MEMBERACCESS_HPP

#include <ir/Printable.hpp>
#include <ir/Variable.hpp>

namespace ct
{

  struct MemberAccess
  {

    Printable onExpression;
    Variable variable;
    bool isArrow;
  };

  bool operator==(const MemberAccess &lhs, const MemberAccess &rhs)
  {
    return lhs.onExpression == rhs.onExpression && lhs.variable == rhs.variable;
  }
  bool operator!=(const MemberAccess &lhs, const MemberAccess &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const MemberAccess &memberAccess)
  {

    print(stream, memberAccess.onExpression);
    stream << (memberAccess.isArrow ? "->" : ".");
    print(stream, memberAccess.variable);
  }

  template <typename Visitor>
  MemberAccess transform(Visitor &&visitor, MemberAccess &&memberAccess)
  {
    return {
        visitor.visit(std::move(memberAccess.onExpression)),
        visitor.visit(std::move(memberAccess.variable))};
  }

  const Type * get_type(const MemberAccess& expr) {
    return get_type(expr.onExpression);
  }

} // namespace ct

#endif /* MEMBERACCESS_HPP */
