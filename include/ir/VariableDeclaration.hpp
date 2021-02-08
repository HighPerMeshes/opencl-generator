#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP


#include <ir/Type.hpp>
#include <ir/Variable.hpp>


#include <optional>

namespace ct
{
  
struct VariableDeclaration {

  Variable variable;
  std::optional<Printable> definition;
  std::vector<Printable> dimensions;

};

bool operator==(const VariableDeclaration& lhs, const VariableDeclaration& rhs) {
  return lhs.variable == rhs.variable && lhs.definition == rhs.definition && vector_equals(lhs.dimensions, rhs.dimensions);
}
bool operator!=(const VariableDeclaration& lhs, const VariableDeclaration& rhs) {
  return !(lhs == rhs);
}

template<typename Stream> 
void print(Stream& stream, const VariableDeclaration& variableDeclaration) {

  const auto& variable = variableDeclaration.variable;
  print(stream, variable.type, " ", variable.name);
  for(const auto& dimension : variableDeclaration.dimensions) {
    print(stream, "[", dimension, "]");
  }
  if (variableDeclaration.definition) {
    print(stream, " = ");
    print(stream, variableDeclaration.definition.value());
  }
}

template<typename Visitor>
VariableDeclaration transform(Visitor&& visitor, VariableDeclaration&& variableDeclaration) {
  return {
    visitor.visit(std::move(variableDeclaration.variable)), 
    visitor.visit(std::move(variableDeclaration.definition)),
    visitor.visit(std::move(variableDeclaration.dimensions))
  };
}

const Type * get_type(const VariableDeclaration& expr) {
    return &expr.variable.type;
}


} // namespace ct


#endif /* VARIABLEDECLARATION_HPP */
