#ifndef C_CONSTRUCTEXPRESSION_HPP
#define C_CONSTRUCTEXPRESSION_HPP

#include <ir/Variable.hpp>
#include <ir/Type.hpp>

namespace ct
{

  struct NamedInit {
    Name name;
    std::vector<Printable> arguments;
  };
 
 bool operator==(const NamedInit &lhs, const NamedInit &rhs)
  {
    return lhs.name == rhs.name && vector_equals(lhs.arguments, rhs.arguments);
  }
  bool operator!=(const NamedInit &lhs, const NamedInit &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const NamedInit &namedInit)
  {
    print(stream, ".", namedInit.name, "={" );
    print_with_delimiter(stream, namedInit.arguments);
    print(stream, "}");
  }

  template <typename Visitor>
  NamedInit transform(Visitor &&visitor, NamedInit &&namedInit)
  {

    return {
        visitor.visit(std::move(namedInit.name)),
        visitor.visit(std::move(namedInit.arguments))
        };
  }

  const Type * get_type(const NamedInit& expr) {
    return nullptr;
  }


  struct C_ConstructExpression
  {

    Type constructs;
    std::vector<NamedInit> arguments;
  };

  bool operator==(const C_ConstructExpression &lhs, const C_ConstructExpression &rhs)
  {
    return lhs.constructs == rhs.constructs && vector_equals(lhs.arguments, rhs.arguments);
  }
  bool operator!=(const C_ConstructExpression &lhs, const C_ConstructExpression &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const C_ConstructExpression &expression)
  {
    print(stream, "(", expression.constructs, ") {");
    print_with_delimiter(stream, expression.arguments);
    print(stream, "}");
  }

  template <typename Visitor>
  C_ConstructExpression transform(Visitor &&visitor, C_ConstructExpression &&constructExpression)
  {

    return {
        visitor.visit(std::move(constructExpression.constructs)),
        visitor.visit(std::move(constructExpression.arguments))};
  }

  const Type * get_type(const C_ConstructExpression& expr) {
    return &expr.constructs;
  }

} // namespace ct

#endif /* CONSTRUCTEXPRESSION_HPP */
