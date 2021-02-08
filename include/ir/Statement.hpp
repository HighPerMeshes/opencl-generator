#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include <ir/Printable.hpp>

namespace ct
{

  struct Statement
  {

    Printable wraps;
  };

  bool operator==(const Statement &lhs, const Statement &rhs)
  {
    return lhs.wraps == rhs.wraps;
  }
  bool operator!=(const Statement &lhs, const Statement &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Statement &statement)
  {
    print(stream, statement.wraps);
    stream << ";";
  }

  template <typename Visitor>
  Statement transform(Visitor &&visitor, Statement &&statement)
  {
    return {visitor.visit(std::move(statement.wraps))};
  }

  const Type * get_type(const Statement&) {
    return nullptr;
  }



} // namespace ct

#endif /* STATEMENT_HPP */
