#ifndef PARENTHESIS_HPP
#define PARENTHESIS_HPP

#include <ir/Printable.hpp>

namespace ct
{

  struct Parenthesis
  {

    Printable wraps;
  };

  bool operator==(const Parenthesis &lhs, const Parenthesis &rhs)
  {
    return lhs.wraps == rhs.wraps;
  }
  bool operator!=(const Parenthesis &lhs, const Parenthesis &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Parenthesis &statement)
  {
    print(stream, "(", statement.wraps, ")");
  }

  template <typename Visitor>
  Parenthesis transform(Visitor &&visitor, Parenthesis &&statement)
  {
    return {visitor.visit(std::move(statement.wraps))};
  }

  const Type * get_type(const Parenthesis& paren) {
    return get_type(paren.wraps);
  }




} // namespace ct

#endif /* PARENTHESIS_HPP */
