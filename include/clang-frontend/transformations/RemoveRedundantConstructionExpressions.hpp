#ifndef REMOVEREDUNDANTCONSTRUCTIONEXPRESSIONS_HPP
#define REMOVEREDUNDANTCONSTRUCTIONEXPRESSIONS_HPP

#include <ir.hpp>

namespace ct
{

auto RemoveRedundantConstructionExpressions()
{
  return DepthFirstTransform{
      printable_case(
        [](ConstructExpression&& expression) {
          if(expression.arguments.size() != 1) {
            return expression;
          }
          auto * nested_expr = printable_get_optional<ConstructExpression>(expression.arguments[0]);
          if(nested_expr) {
            return *nested_expr;
          }
          return expression;
        }
      )
    };
}

} // namespace ct

#endif /* REMOVEREDUNDANTCONSTRUCTIONEXPRESSIONS_HPP */
