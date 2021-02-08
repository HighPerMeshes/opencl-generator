#ifndef CAST_HPP
#define CAST_HPP

#include <ir/Printable.hpp>
#include <ir/Type.hpp>

namespace ct
{

  struct Cast
  {
    Type castTo;
    Printable onExpression;
  };

  bool operator==(const Cast &lhs, const Cast &rhs)
  {
    return lhs.castTo == rhs.castTo && lhs.onExpression == rhs.onExpression;
  }
  bool operator!=(const Cast &lhs, const Cast &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Cast &cast)
  {
    print(stream, "(", cast.castTo, ")", cast.onExpression);
  }

  template <typename Visitor>
  Cast transform(Visitor &&visitor, Cast &&cast)
  {
    return {
        visitor.visit(std::move(cast.castTo)),
        visitor.visit(std::move(cast.onExpression))};
  }

  const Type * get_type(const Cast&) {
    return nullptr;
  }

} // namespace ct
#endif /* NAMEDCAST_HPP */
