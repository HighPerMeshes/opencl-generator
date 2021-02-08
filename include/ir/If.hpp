#ifndef CT_IF_HPP
#define CT_IF_HPP

#include <ir/Printable.hpp>
#include <ir/Statement.hpp>
#include <ir/Block.hpp>

namespace ct
{

  struct If
  {

    std::optional<Printable> init;
    Printable condition;
    Block true_branch;
    Block false_branch;
  };

  bool operator==(const If &lhs, const If &rhs)
  {
    return lhs.init == rhs.init &&
           lhs.condition == rhs.condition &&
           lhs.true_branch == rhs.true_branch &&
           lhs.false_branch == rhs.false_branch;
  }
  bool operator!=(const If &lhs, const If &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const If &if_condition)
  {
    print(stream, "if(");
    if (if_condition.init)
    {
      print(stream, if_condition.init, "; ");
    }
    print(stream, if_condition.condition, ")\n", if_condition.true_branch, "\n");
    if (!if_condition.false_branch.body.empty())
    {
      print(stream, "else \n", if_condition.false_branch);
    }
  }

  template <typename Visitor>
  If transform(Visitor &&visitor, If &&if_condition)
  {
    return {
        visitor.visit(std::move(if_condition.init)),
        visitor.visit(std::move(if_condition.condition)),
        visitor.visit(std::move(if_condition.true_branch)),
        visitor.visit(std::move(if_condition.false_branch))};
  }

  const Type * get_type(const If&) {
    return nullptr;
  }
  

} // namespace ct

#endif /* IF_HPP */
