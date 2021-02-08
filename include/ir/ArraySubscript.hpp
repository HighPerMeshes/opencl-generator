#ifndef ARRAYSUBSCRIPT_HPP
#define ARRAYSUBSCRIPT_HPP

#include <ir/Printable.hpp>

namespace ct
{

  struct ArraySubscript
  {

    Printable onExpression;
    Printable index;
  };

  bool operator==(const ArraySubscript &lhs, const ArraySubscript &rhs)
  {
    return lhs.index == rhs.index && lhs.onExpression == rhs.onExpression;
  }
  bool operator!=(const ArraySubscript &lhs, const ArraySubscript &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const ArraySubscript &subscript)
  {
    print(stream, subscript.onExpression, "[", subscript.index, "]");
  }

  template <typename Visitor>
  ArraySubscript transform(Visitor &&visitor, ArraySubscript &&subscript)
  {
    return {
        visitor.visit(std::move(subscript.onExpression)),
        visitor.visit(std::move(subscript.index))};
  }

  const Type * get_type(const ArraySubscript& subscript) {
    return get_type(subscript.onExpression);
  }


} // namespace ct

#endif /* ARRAYSUBSCRIPT_HPP */
