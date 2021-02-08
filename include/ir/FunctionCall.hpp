#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <ir/Printable.hpp>
#include <ir/Name.hpp>
#include <ir/Type.hpp>
#include <ir/Function.hpp>

#include <util.hpp>

#include <vector>

namespace ct
{

  struct FunctionCall
  {

    Type return_type;
    Name name;
    std::vector<Printable> arguments;
    std::vector<Printable> template_arguments;

    std::optional<Function> function;
  };

  bool operator==(const FunctionCall &lhs, const FunctionCall &rhs)
  {
    return lhs.return_type == rhs.return_type, lhs.name == rhs.name && vector_equals(lhs.arguments, rhs.arguments) && vector_equals(lhs.template_arguments, rhs.template_arguments) && lhs.function == rhs.function;
  }
  bool operator!=(const FunctionCall &lhs, const FunctionCall &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const FunctionCall &functionCall)
  {
    print(stream, functionCall.name);
    if (!functionCall.template_arguments.empty())
    {
      stream << "<";
      print_with_delimiter(stream, functionCall.template_arguments);
      stream << ">";
    }
    stream << "(";
    print_with_delimiter(stream, functionCall.arguments);
    stream << ")";
  }

  template <typename Visitor>
  FunctionCall transform(Visitor &&visitor, FunctionCall &&call)
  {
    return {
        visitor.visit(std::move(call.return_type)),
        visitor.visit(std::move(call.name)),
        visitor.visit(std::move(call.arguments)),
        visitor.visit(std::move(call.template_arguments)),
        visitor.visit(std::move(call.function))};
  }


  const Type * get_type(const FunctionCall& call) {
    return &call.return_type;
  }

} // namespace ct

#endif /* FUNCTIONCALL_HPP */
