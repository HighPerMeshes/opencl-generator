#ifndef VEC2FLOATNTRANSFORM_HPP
#define VEC2FLOATNTRANSFORM_HPP

#include <ir.hpp>
#include <regex>

namespace ct
{

  auto MakeVector(size_t dim)
  {

    VariableDeclaration ptr_decl{Variable{Type{Name{std::string{"double"}}}.with(Pointer()).with(Const()), Name{"ptr"}}};

    std::vector<Statement> body;

    C_ConstructExpression constructs{
        Type{Name{std::string{"struct Vector_double_"} + std::to_string(dim)}},
        std::vector<NamedInit>{
            NamedInit{
                Name{"values"}}}};

    for (size_t i = 0; i < dim; ++i)
    {

      ArraySubscript ptr_access{make_printable(ptr_decl.variable), make_printable(Literal{Int(), std::to_string(i)})};

      constructs.arguments[0].arguments.emplace_back(
          make_printable(ptr_access));
    }

    body.emplace_back(Statement{make_printable(Return{
        make_printable(constructs)})});

    return Function{
        Type{Name{std::string{"struct Vector_double_"} + std::to_string(dim)}},
        Name{std::string{"make_Vector_double_"} + std::to_string(dim)},
        std::vector{ptr_decl},
        Block{body}};
  }

  auto AssignVector(size_t dim)
  {

    VariableDeclaration vec_decl{Variable{Type{Name{std::string{"struct Vector_double_"} + std::to_string(dim)}}, Name{"vec"}}};
    VariableDeclaration ptr_decl{Variable{Type{Name{std::string{"double"}}}.with(Pointer()).with(Const()), Name{"ptr"}}};

    std::vector<Statement> body;
    for (size_t i = 0; i < dim; ++dim)
    {

      ArraySubscript vec_access{make_printable(vec_decl.variable), make_printable(Literal{Int(), std::to_string(i)})};
      ArraySubscript ptr_access{make_printable(ptr_decl.variable), make_printable(Literal{Int(), std::to_string(i)})};

      Statement stmt{
          make_printable(
              BinaryOperator{
                  make_printable(vec_access),
                  "=",
                  make_printable(ptr_access)})};

      body.emplace_back(stmt);
    }

    return Function{
        Type{Name{std::string{"struct Vector_double_"} + std::to_string(dim)}},
        Name{std::string{"make_Vector_double_"} + std::to_string(dim)},
        std::vector{vec_decl, ptr_decl},
        Block{body}};
  }

  auto Vec2Vec(std::set<size_t> &needed_vectors)
  {

    auto Vec2FloatN = [&](Type &&type) {
      if (type.name.name != "HPM::dataType::Vec")
        return type;

      std::stringstream name_stream;
      name_stream << "struct Vector_";
      print(name_stream, type.template_arguments[0]);

      std::stringstream ss;
      print(ss, type.template_arguments[1]);

      name_stream << "_" << ss.str();

      needed_vectors.insert(std::stoi(ss.str()));

      type.name.name = name_stream.str();
      type.template_arguments = std::vector<Printable>{};

      return type;
    };

    return Transformer{
        overloaded{
            printable_case(Vec2FloatN),
            Vec2FloatN}};
  }

  auto Mat2Mat(std::set<std::pair<size_t, size_t>> &needed_matrices)
  {

    auto Mat2FloatN = [&](Type &&type) {
      if (type.name.name != "HPM::dataType::Matrix")
        return type;

      std::stringstream name_stream;
      name_stream << "struct Matrix_";

      print(name_stream, type.template_arguments[0]);

      std::stringstream N;
      print(N, type.template_arguments[1]);

      std::stringstream M;
      print(M, type.template_arguments[2]);

      needed_matrices.emplace(std::stoi(N.str()), std::stoi(M.str()));

      name_stream << "_" << N.str() << "_" << M.str();

      type.name = Name{name_stream.str()};
      type.template_arguments = std::vector<Printable>{};

      return type;
    };

    return Transformer{
        overloaded{
            printable_case(Mat2FloatN),
            Mat2FloatN}};
  }

  auto FixVecSubscripts()
  {

    return DepthFirstTransform{
        printable_case(
            [](ArraySubscript &&subscript) {
              auto *type = get_type(subscript.onExpression);

              if (!type)
                return make_printable(subscript);
              auto name = type->name.name;
              if (name.find("struct Vector_") == std::string::npos && name.find("struct Matrix_") == std::string::npos)
                return make_printable(subscript);

              // auto *get = printable_get_optional<ArraySubscript>(subscript.onExpression);
              // if (get)
              //   return make_printable(subscript);

              MemberAccess access{
                  subscript.onExpression,
                  Variable{Type{Name{"double"}}, Name{"values"}},
                  type->is(Pointer())};

              subscript.onExpression = make_printable(access);

              return make_printable(subscript);
            })};
  }

  auto VectorCreation()
  {
    return Transformer{
        printable_case(
            [](ConstructExpression &&expr) {
              if (expr.constructs.name.name.find("struct Vector_") == std::string::npos)
                return make_printable(expr);

              if (expr.arguments.size() == 1)
                return make_printable(expr);

              return make_printable(C_ConstructExpression{
                  expr.constructs,
                  std::vector<NamedInit>{
                      NamedInit{
                          Name{"values"},
                          expr.arguments}}});
            })};
  }

  auto ZeroInit()
  {
    return Transformer{
        printable_case(
            [](VariableDeclaration &&decl) {
              auto name = decl.variable.type.name.name;

              if (name.find("struct Vector_") == std::string::npos && name.find("struct Matrix_") == std::string::npos)
              {
                return decl;
              }

              if (decl.definition)
              {
                return decl;
              }

              decl.definition = make_printable(ConstructExpression{decl.variable.type, std::vector{make_printable(Literal{Int(), "0"})}, true});

              return decl;
            })};
  }

} // namespace ct

#endif /* VEC2FLOATNTRANSFORM_HPP */
