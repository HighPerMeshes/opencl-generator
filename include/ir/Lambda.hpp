#ifndef LAMBDAEXPR_HPP
#define LAMBDAEXPR_HPP

#include <ir/Printable.hpp>
#include <ir/Statement.hpp>
#include <ir/VariableDeclaration.hpp>
#include <ir/Block.hpp>

#include <util.hpp>

#include <vector>

namespace ct
{

  struct Lambda
  {

    struct Capture
    {

      Printable captures;
      bool by_reference;
    };

    struct DefaultCapture
    {

      std::string_view representation;
    };

    static constexpr DefaultCapture none{""};
    static constexpr DefaultCapture by_reference{"&"};
    static constexpr DefaultCapture by_value{"="};

    DefaultCapture defaultCapture;
    std::vector<Capture> captures;
    std::vector<VariableDeclaration> parameters;
    Block body;
  };

  bool operator==(const Lambda::Capture &lhs, const Lambda::Capture &rhs)
  {
    return lhs.by_reference == rhs.by_reference && lhs.captures == rhs.captures;
  }
  bool operator!=(const Lambda::Capture &lhs, const Lambda::Capture &rhs)
  {
    return !(lhs == rhs);
  }

  bool operator==(const Lambda &lhs, const Lambda &rhs)
  {
    return lhs.defaultCapture.representation == rhs.defaultCapture.representation && vector_equals(lhs.captures, rhs.captures) && vector_equals(lhs.parameters, rhs.parameters) && lhs.body == rhs.body;
  }
  bool operator!=(const Lambda &lhs, const Lambda &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Lambda::DefaultCapture &defaultCapture)
  {
    stream << defaultCapture.representation;
  }

  template <typename Stream>
  void print(Stream &stream, const Lambda::Capture &capture)
  {
    stream << (capture.by_reference ? "&" : "");
    print(stream, capture.captures);
  }

  template <typename Stream>
  void print(Stream &stream, const Lambda &lambda)
  {

    using namespace std;
    stream << "[";
    print(stream, lambda.defaultCapture);
    if (lambda.defaultCapture.representation != Lambda::none.representation && !lambda.captures.empty())
    {
      stream << ", ";
    }
    print_with_delimiter(stream, lambda.captures);
    stream << "](";
    print_with_delimiter(stream, lambda.parameters);
    print(stream, ")\n", lambda.body);
  }

  template <typename Visitor>
  Lambda::Capture transform(Visitor &&visitor, Lambda::Capture &&capture)
  {
    return {
        visitor.visit(std::move(capture.captures)), capture.by_reference};
  }

  template <typename Visitor>
  Lambda transform(Visitor &&visitor, Lambda &&lambda)
  {
    return {
        visitor.visit(std::move(lambda.defaultCapture)),
        visitor.visit(std::move(lambda.captures)),
        visitor.visit(std::move(lambda.parameters)),
        visitor.visit(std::move(lambda.body))};
  }

  const Type * get_type(const Lambda&) {
    return nullptr;
  }
  const Type * get_type(const Lambda::Capture&) {
    return nullptr;
  }


} // namespace ct

#endif /* LAMBDAEXPR_HPP */
