#ifndef USINGS_HPP
#define USINGS_HPP

#include <string>
#include <memory>
#include <util/PolymorhicValue.hpp>
#include <iostream>

namespace ct
{

  using StringType = std::string;

  struct AddressOf;
  struct ArraySubscript;
  struct BinaryOperator;
  struct Block;
  struct Cast;
  struct C_ConstructExpression;
  struct ConstructExpression;
  struct ForLoop;
  struct Function;
  struct FunctionCall;
  struct If;
  struct InitConditionIncrement;
  struct Lambda;
  struct Literal;
  struct MemberAccess;
  struct MemberCall;
  struct Name;
  struct NamedCast;
  struct NamedInit;
  struct NotHandled;
  struct NullStmt;
  struct Parenthesis;
  struct Qualifier;
  struct Range;
  struct Return;
  struct Statement;
  struct TemplateTemplate;
  struct ThisPtr;
  struct Type;
  struct UnaryOperator;
  struct Unknown;
  struct Variable;
  struct VariableDeclaration;

  template <typename... Ts>
  using TermVariant = std::variant<Term<Ts>...>;

  using Printable = TermVariant<
      AddressOf,
      ArraySubscript,
      BinaryOperator,
      Block,
      Cast,
      C_ConstructExpression,
      ConstructExpression,
      ForLoop,
      Function,
      FunctionCall,
      If,
      InitConditionIncrement,
      Lambda,
      Literal,
      MemberAccess,
      MemberCall,
      Name,
      NamedCast,
      NamedInit,
      NotHandled,
      NullStmt,
      Parenthesis,
      Qualifier,
      Range,
      Return,
      Statement,
      TemplateTemplate,
      ThisPtr,
      Type,
      UnaryOperator,
      Unknown,
      Variable,
      VariableDeclaration>;

  template <typename Stream>
  void print(Stream &stream, const Printable &printable)
  {
    match(
        printable,
        [&stream](const auto &value) { print(stream, value.get()); });
  }

  template <typename T, typename... Args>
  auto make_printable(Args &&... args) -> Printable
  {
    return Printable{std::in_place_type<Term<T>>, std::forward<Args>(args)...};
  }

  template <typename T>
  auto make_printable(T &&t) -> Printable
  {
    return Printable{std::in_place_type<Term<std::decay_t<T>>>, std::forward<T>(t)};
  }

  auto make_printable(Printable printable) -> Printable
  {
    return printable;
  }

  template <typename T>
  auto printable_get(const Printable &printable, std::string failure_message) -> const T &
  {
    auto *got = match(
        printable,
        [](const Term<T> &term) -> const T * { return &term.get(); },
        [](auto &&) -> const T * { return nullptr; });
    if (!got)
    {
      print(std::cout, failure_message, ":\n", printable, "\n");
      fail_with_message("");
    }
    return *got;
  }

  template <typename T>
  auto printable_get_optional(Printable &printable) -> T *
  {
    return match(
        printable,
        [](Term<T> &term) -> T * { return &term.get(); },
        [](auto &&) -> T * { return nullptr; });
  }

  template <typename T>
  auto printable_get_optional(const Printable &printable) -> const T *
  {
    return match(
        printable,
        [](const Term<T> &term) -> const T * { return &term.get(); },
        [](auto &&) -> const T * { return nullptr; });
  }

  template <typename Visitor>
  Printable transform(Visitor &&visitor, Printable &&printable)
  {
    return match(
        printable,
        [&visitor](auto &&value) {
          return make_printable(visitor.visit(std::move(value.get())));
        });
  }

  const Type * get_type(const Printable& printable) {
    
    return match(
        printable,
        [](const auto &value) -> const Type * {
          return get_type(value.get());
        });
  }



} // namespace ct

#endif /* USINGS_HPP */
