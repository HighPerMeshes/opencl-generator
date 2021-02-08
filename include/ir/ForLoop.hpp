#ifndef FORLOOP_HPP
#define FORLOOP_HPP

#include <ir/Printable.hpp>
#include <ir/VariableDeclaration.hpp>
#include <ir/Block.hpp>

namespace ct
{

  struct InitConditionIncrement
  {

    Printable initializer;
    Printable condition;
    Printable increment;
  };

  template <typename Stream>
  void print(Stream &stream, const InitConditionIncrement &head)
  {
    print(stream, head.initializer, "; ", head.condition, "; ", head.increment);
  }

  template <typename Visitor>
  InitConditionIncrement transform(Visitor &&visitor, InitConditionIncrement &&head)
  {
    return {
        visitor.visit(std::move(head.initializer)),
        visitor.visit(std::move(head.condition)),
        visitor.visit(std::move(head.increment))};
  }

  bool operator==(const InitConditionIncrement &lhs, const InitConditionIncrement &rhs)
  {
    return lhs.initializer == rhs.initializer &&
           lhs.condition == rhs.condition &&
           lhs.increment == rhs.increment;
  }

  bool operator!=(const InitConditionIncrement &lhs, const InitConditionIncrement &rhs)
  {
    return !(lhs == rhs);
  }


  const Type * get_type(const InitConditionIncrement&) {
    return nullptr;
  }

  struct Range
  {

    VariableDeclaration variable;
    Printable from;
  };

  template <typename Stream>
  void print(Stream &stream, const Range &range)
  {
    print(stream, range.variable, ": ", range.from);
  }

  bool operator==(const Range &lhs, const Range &rhs)
  {
    return lhs.variable == rhs.variable &&
           lhs.from == rhs.from;
  }

  bool operator!=(const Range &lhs, const Range &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Visitor>
  Range transform(Visitor &&visitor, Range &&range)
  {
    return {
        visitor.visit(std::move(range.variable)),
        visitor.visit(std::move(range.from))};
  }

  const Type * get_type(const Range&) {
    return nullptr;
  }

  struct ForLoop
  {

    Printable header;
    Block body;
  };

  bool operator==(const ForLoop &lhs, const ForLoop &rhs)
  {
    return lhs.header == rhs.header &&
           lhs.body == rhs.body;
  }

  bool operator!=(const ForLoop &lhs, const ForLoop &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const ForLoop &forLoop)
  {
    print(stream, "for(", forLoop.header, ")\n");
    print(stream, forLoop.body);
  }

  template <typename Visitor>
  ForLoop transform(Visitor &&visitor, ForLoop &&forLoop)
  {
    return {
        visitor.visit(std::move(forLoop.header)),
        visitor.visit(std::move(forLoop.body))};
  }

  const Type * get_type(const ForLoop&) {
    return nullptr;
  }

} // namespace ct

#endif /* FORLOOP_HPP */
