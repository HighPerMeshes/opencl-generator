#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <ir/Name.hpp>
#include <ir/Statement.hpp>
#include <ir/Type.hpp>
#include <ir/VariableDeclaration.hpp>
#include <ir/Block.hpp>

#include <util.hpp>

#include <vector>

namespace ct
{

  struct Function
  {
    Type return_type;
    Name name;
    std::vector<VariableDeclaration> parameters;
    std::optional<Block> body;
  };

  Function empty_function() {
    return {
      Type{Name{""}},
      Name{""}
    };
  }


  bool operator==(const Function &lhs, const Function &rhs)
  {
    return lhs.return_type == rhs.return_type && lhs.name == rhs.name && vector_equals(lhs.parameters, rhs.parameters) && lhs.body == rhs.body;
  }
  bool operator!=(const Function &lhs, const Function &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Function &function)
  {

    print(stream, function.return_type, " ", function.name, "(");
    print_with_delimiter(stream, function.parameters);
    print(stream, ")\n");
    if(function.body) {
      print(stream, function.body);
    }
    else print(stream, ";");
  }

  template <typename Visitor>
  Function transform(Visitor &&visitor, Function &&function)
  {
    return {
        visitor.visit(std::move(function.return_type)),
        visitor.visit(std::move(function.name)),
        visitor.visit(std::move(function.parameters)),
        visitor.visit(std::move(function.body))};
  }

  const Type * get_type(const Function& func) {
    return &func.return_type;
  }
  

} // namespace ct

#endif // KERNEL_HPP