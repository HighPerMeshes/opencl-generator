#ifndef CONSTRUCTEXPRESSION_HPP
#define CONSTRUCTEXPRESSION_HPP

#include <ir/Variable.hpp>
#include <ir/Type.hpp>

namespace ct
{

  struct ConstructExpression
  {

    Type constructs;
    std::vector<Printable> arguments;
    bool isInitList;
  };

  bool operator==(const ConstructExpression &lhs, const ConstructExpression &rhs)
  {
    return lhs.constructs == rhs.constructs && lhs.isInitList == rhs.isInitList && vector_equals(lhs.arguments, rhs.arguments);
  }
  bool operator!=(const ConstructExpression &lhs, const ConstructExpression &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const ConstructExpression &expression)
  {
    print(stream, expression.isInitList ? "{" : "(");
    print_with_delimiter(stream, expression.arguments);
    print(stream, expression.isInitList ? "}" : ")");
  }

  template <typename Visitor>
  ConstructExpression transform(Visitor &&visitor, ConstructExpression &&constructExpression)
  {

    return {
        visitor.visit(std::move(constructExpression.constructs)),
        visitor.visit(std::move(constructExpression.arguments)),
        visitor.visit(std::move(constructExpression.isInitList))};
  }

  const Type * get_type(const ConstructExpression& expr) {
    return &expr.constructs;
  }

} // namespace ct

#endif /* CONSTRUCTEXPRESSION_HPP */
