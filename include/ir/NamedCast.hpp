#ifndef NAMEDCAST_HPP
#define NAMEDCAST_HPP

#include <ir/Printable.hpp>
#include <ir/Type.hpp>
#include <ir/Name.hpp>

namespace ct
{

  struct NamedCast
  {
    Name name;
    Type castTo;
    Printable onExpression;
  };

  bool operator==(const NamedCast &lhs, const NamedCast &rhs)
  {
    return lhs.name == rhs.name && lhs.castTo == rhs.castTo && lhs.onExpression == rhs.onExpression;
  }
  bool operator!=(const NamedCast &lhs, const NamedCast &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const NamedCast &cast)
  {
    print(stream, cast.name, "<", cast.castTo, ">(", cast.onExpression ,")");
  }

  template <typename Visitor>
  NamedCast transform(Visitor &&visitor, NamedCast &&cast)
  {
    return {
        visitor.visit(std::move(cast.name)),
        visitor.visit(std::move(cast.castTo)),
        visitor.visit(std::move(cast.onExpression))};
  }

  const Type * get_type(const NamedCast&) {
    return nullptr;
  }

} // namespace ct
#endif /* NAMEDCAST_HPP */
