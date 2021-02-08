#ifndef MEMBERCALL_HPP
#define MEMBERCALL_HPP

#include <ir/FunctionCall.hpp>
#include <ir/Variable.hpp>

namespace ct
{

  struct MemberCall
  {

    Printable onExpression;
    FunctionCall call;
  };

  bool operator==(const MemberCall &lhs, const MemberCall &rhs)
  {
    return lhs.onExpression == rhs.onExpression && lhs.call == rhs.call;
  }
  bool operator!=(const MemberCall &lhs, const MemberCall &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const MemberCall &memberCall)
  {

    print(stream, memberCall.onExpression);
    stream << ".";
    if (!memberCall.call.template_arguments.empty())
      stream << "template ";
    print(stream, memberCall.call);
  }

  template <typename Visitor>
  MemberCall transform(Visitor &&visitor, MemberCall &&call)
  {
    return {
        visitor.visit(std::move(call.onExpression)),
        visitor.visit(std::move(call.call))};
  }

  const Type * get_type(const MemberCall& call) {
    return get_type(call.call);
  }

} // namespace ct

#endif /* MEMBERCALL_HPP */
