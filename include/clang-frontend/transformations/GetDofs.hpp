#ifndef RESOLVEGETDOFS_HPP
#define RESOLVEGETDOFS_HPP

#include <ir.hpp>
#include <numeric>
#include <algorithm>

namespace ct
{

template <typename Printable>
void unique_vector(std::vector<Printable> &vector)
{
  std::sort(vector.begin(), vector.end(),
            [](const auto &lhs, const auto &rhs) {
              std::stringstream lhs_stream;
              print(lhs_stream, lhs);

              std::stringstream rhs_stream;
              print(rhs_stream, rhs);

              return lhs_stream.str() < rhs_stream.str();
            });
  auto last = std::unique(vector.begin(), vector.end());
  vector.erase(last, vector.end());
}


constexpr auto dof2Variable = [](const Type &type) {
    assert(type.template_arguments.size() > 1);

    using namespace std;

    auto name = accumulate(
        next(begin(type.template_arguments)),
        end(type.template_arguments),
        string{"dof"},
        [](const string &name, const Printable &printable) {
          stringstream stream;
          print(stream, "_");
          print(stream, printable);

          return name + stream.str();
        });

    return Variable{
        Type{Name{"size_t"}, {}, {Const()}},
        Name{name}};
  };


auto dof2Declaration(const Type &type) {
    assert(type.template_arguments.size() > 1);

    auto variable = dof2Variable(type);

    using namespace std;

    vector<Printable> args;
    args.reserve(type.template_arguments.size() - 1);
    copy(
        next(begin(type.template_arguments)),
        end(type.template_arguments),
        back_inserter(args));

    auto args_size = args.size();

    auto decl = VariableDeclaration{
        variable,
        make_printable(ConstructExpression{ variable.type, std::move(args), true}),
        std::vector{ make_printable(Variable { Int(),  std::to_string(args_size) } ) }};

    return decl;
  }

auto HandleDofs()
{

  return Transformer{
      [](Function&& function) {

          if(!function.body) return function;

          std::vector<Statement> array_declarations;

          function.body = MakeTransformUntil<Function>(
            printable_case([&array_declarations](VariableDeclaration &&decl) {
            if (decl.variable.type.name.name != "HPM::internal::LocalBuffer")
              return decl;

            auto &buffer = printable_get<Type>(
                decl.variable.type.template_arguments[0],
                "GetDofs: buffer could not be found");

            auto &dofs = printable_get<Type>(
                buffer.template_arguments[2],
                "GetDofs: dof type could not be found");

            array_declarations.emplace_back(Statement{make_printable(dof2Declaration(dofs))});

            return decl;
          })).visit(function.body);
        
          unique_vector(array_declarations);

          function.body.value().body.insert(
            function.body.value().body.begin(),
            array_declarations.begin(),
            array_declarations.end()
          );

          return function;
}};

      
}

} // namespace ct

#endif /* RESOLVEGETDOFS_HPP */
