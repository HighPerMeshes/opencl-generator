#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <util/PolymorhicValue.hpp>
#include <test_util.hpp>
#include <CT.hpp>

TEST_CASE("match works", "[match]")
{

  using A = HasValue<struct A_ID>;
  using B = HasValue<struct B_ID>;

  using namespace ct;
  using Value = std::variant<A, B>;

  Value a = make_variant<Value, A>(42);

  SECTION("make_variant creates correct variant")
  {

    REQUIRE(std::get<A>(a).value == 42);
  }

  SECTION("matches correct overload")
  {

    Counter c{3};

    match(
        a,
        [&c](const A &a) {
          REQUIRE(a.value == 42);
          c++;
        },
        [](const B &b) { FAIL("Can not hold an instance of B at this point"); });

    match(
        a,
        [&c](A &a) {
          REQUIRE(a.value == 42);
          c++;
        },
        [](auto &&) { FAIL("Should match the first lambda"); });

    match(
        a,
        [](auto &&) { FAIL("Should match the second lambda"); },
        [&c](A &a) {
          REQUIRE(a.value == 42);
          c++;
        });
  }

  SECTION("Default is not used if value exists in case of void")
  {

    match(
        a,
        [](A &a) { a.value = 100; },
        Default());

    auto val = std::get<A>(a);
    REQUIRE(val.value == 100);
  }

  SECTION("Default is not used if value exists")
  {

    auto val = match(
        a,
        [](A &a) { return a.value; },
        Default(666));

    REQUIRE(val == 42);
  }

  SECTION("Default is used if value does not exist")
  {

    auto val = match(
        a,
        [](B &a) { return a.value; },
        Default(666));

    REQUIRE(val == 666);
  }
}

TEST_CASE("Term works", "[term]")
{
  using namespace ct;

  Term<int> term{10};

  SECTION("term can be copied")
  {
    Term<int> copy{term};
    REQUIRE(term.get() == 10);
    REQUIRE(copy.get() == 10);
  }

  SECTION("term can be moved")
  {
    Term<int> copy{std::move(term)};
    REQUIRE(copy.get() == 10);
  }

  SECTION("term can be assigned moved")
  {
    Term<int> assigned;
    assigned = term;
    REQUIRE(term.get() == 10);
    REQUIRE(assigned.get() == 10);
  }

  SECTION("term can be move assigned")
  {
    Term<int> assigned;
    assigned = std::move(term);
    REQUIRE(assigned.get() == 10);
  }

  struct NoCopy
  {

    NoCopy() = default;
    NoCopy(const NoCopy &) = delete;
    NoCopy(NoCopy &&) = default;
    NoCopy &operator=(const NoCopy &) = delete;
    NoCopy &operator=(NoCopy &&) = default;
    ~NoCopy() = default;
  };

  SECTION("NoCopy can be moved")
  {
    NoCopy no;
    NoCopy no2{std::move(no)};
  }

  Term<NoCopy> nocopy_term;

  SECTION("NoCopy can be moved in a term")
  {
    Term<NoCopy> moved{std::move(nocopy_term)};
  }

  SECTION("NoCopy can be move assigned in a term")
  {
    Term<NoCopy> moved;
    moved = std::move(nocopy_term);
  }
}

TEMPLATE_TEST_CASE("Transforming Printables work", "[Printable]",
                   ct::BinaryOperator,
                   ct::ConstructExpression,
                   ct::FunctionCall,
                   ct::Kernel,
                   ct::Lambda,
                   ct::Literal,
                   ct::MemberCall,
                   ct::Name,
                   ct::Qualifier,
                   ct::Return,
                   ct::Statement,
                   ct::TemplateTemplate,
                   ct::Type,
                   ct::Unknown,
                   ct::Variable,
                   ct::VariableDeclaration)
{

  using namespace ct;
  auto v = ct::make_printable(make<TestType>());

  SECTION("can match printables")
  {

    Counter c{1};

    match(
        std::move(v),
        [&c](Term<TestType> &val) { c++; },
        [](auto &&val) {});

    bool res = match(
        std::move(v),
        [&c](Term<TestType> &val) { return true; },
        [](auto &&val) { return false; });

    REQUIRE(res);
  }

  SECTION("can transform the printable values")
  {

    Counter c{2};

    Transformer t{
        [&c](TestType &&t) {
          c++;
          return t;
        }};

    auto resulting_variant = t.visit(v);
    match(
        resulting_variant,
        [&c](Term<TestType> &val) {
          c++;
          REQUIRE(val.get() == make<TestType>());
        },
        [](auto &&) { FAIL("Should not be able to reach this match"); });
  }

  SECTION("can transform with printable_case")
  {

    Counter c{2};

    Transformer t{
      printable_case([&c](TestType&& t){ 
        c++;
        return make_printable(t); 
      })
    };

    auto resulting_variant = t.visit(v);
    match(
        resulting_variant,
        [&c](Term<TestType> &val) {
          c++;
          REQUIRE(val.get() == make<TestType>());
        },
        [](auto &&) { FAIL("Should not be able to reach this match"); });
  }
}

TEST_CASE("match with predicate", "[Printable]") {

  // using namespace ct;
  // auto v = make<BinaryOperator>();
  
  // auto predicate = [](BinaryOperator& val){ 
  //   return val.op == "+";
  // };

  // Transformer t{
  //   [predicate](BinaryOperator& val) {
  //     if(predicate)

  //     return val;
  //   }
  // };

  // auto resulting t.visit(v);

}
