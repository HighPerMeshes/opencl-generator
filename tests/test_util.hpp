#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP

#include <CT.hpp>

template <typename T>
struct DEBUG;

template <typename ID>
struct HasValue
{
  HasValue(int value) : value{value} {}
  int value;
};

struct Counter
{

  Counter(int goal) : goal{goal} {}

  const int goal;
  int count{0};

  Counter& operator++(int)
  {
    count++;
    return *this;
  }

  ~Counter()
  {
    REQUIRE(goal == count);
  }
};

namespace ct {

  template<typename T>
  T make();

  template<> Name make<Name>() {
    return { "name" };
  }

  template<> Type make<Type>() {
    return { make<Name>(), {}, {} };
  }

  template<> BinaryOperator make<BinaryOperator>() {
    return { make_printable<Unknown>("lhs"), "=", make_printable<Unknown>("rhs") };
  }

  template<> ConstructExpression make<ConstructExpression>() {
    return { make<Type>(), {}, false };
  }

  template<> FunctionCall make<FunctionCall>() {
    return { make<Name>(), {}, {} };
  }

  template<> Kernel make<Kernel>() {
    return { make<Name>(), {}, {} };
  }
  
  template<> Lambda make<Lambda>() {
    return { Lambda::none, {}, {}, {} };
  }

  template<> Literal make<Literal>() {
    return { make<Type>(), {} };
  }

  template<> Variable make<Variable>() {
    return { make<Type>(), make<Name>() };
  }

  template<> MemberCall make<MemberCall>() {
    return { make_printable<Unknown>("onExpression"), make<FunctionCall>() };
  }

  template<> Qualifier make<Qualifier>() {
    return { "&" };
  }

  template<> Return make<Return>() {
    return {};
  }

  template<> Statement make<Statement>() {
    return { make_printable<Unknown>("body") };
  }

  template<> TemplateTemplate make<TemplateTemplate>() {
    return { make<Name>() };
  }

  template<> Unknown make<Unknown>() {
    return { "unknown" };
  }

  template<> VariableDeclaration make<VariableDeclaration>() {
    return { make<Variable>(), {} };
  }

}

#endif