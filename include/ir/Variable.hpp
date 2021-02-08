#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <ir/Name.hpp>
#include <ir/Printable.hpp>
#include <ir/Type.hpp>


namespace ct {
struct Variable {

  Type type;
  Name name;

};

bool operator==(const Variable& lhs, const Variable& rhs) {
  return lhs.type == rhs.type && lhs.name == rhs.name ;
}
bool operator!=(const Variable& lhs, const Variable& rhs) {
  return !(lhs == rhs);
}

template<typename Stream> 
void print(Stream& stream, const Variable& variable) { print(stream, variable.name); }

template<typename Visitor>
Variable transform(Visitor&& visitor, Variable&& variable) {
  
  return {
    visitor.visit(std::move(variable.type)), 
    visitor.visit(std::move(variable.name))
  };
}

const Type * get_type(const Variable& variable) {
  return &variable.type;
}



} // namespace ct

#endif /* VARIABLE_HPP */
