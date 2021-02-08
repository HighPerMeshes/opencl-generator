#ifndef LITERAL_HPP
#define LITERAL_HPP

#include <ir/Printable.hpp>
#include <ir/Type.hpp>

namespace ct
{

  struct Literal
  {

    Type type;
    StringType value;
  };

  bool operator==(const Literal &lhs, const Literal &rhs)
  {
    return lhs.type == rhs.type && lhs.value == rhs.value;
  }
  bool operator!=(const Literal &lhs, const Literal &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Literal literal)
  {
    stream << literal.value;
  }

  template <typename Visitor>
  Literal transform(Visitor &&visitor, Literal &&literal)
  {
    return {
        visitor.visit(std::move(literal.type)),
        visitor.visit(std::move(literal.value))};
  }

  const Type * get_type(const Literal& expr) {
    return &expr.type;
  }

} // namespace ct

#endif /* LITERAL_HPP */
