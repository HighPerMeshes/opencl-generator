#ifndef EXTRACTMEMBERCALLS_HPP
#define EXTRACTMEMBERCALLS_HPP

#include <ir.hpp>
#include <clang-frontend/transformations/UnqualifyNames.hpp>

namespace ct
{

  auto ExtractMemberCalls(std::vector<std::string>& member_names)
  {
    return Transformer{
            [&](FunctionCall && call) {

              if(!call.function) return call;
              auto& function = call.function.value();

              std::vector<VariableDeclaration> new_parameters;

              function.body = MakeTransformUntil<Function>( 
                printable_case(
                [&new_parameters](MemberCall &&call) {

                if (auto *type = get_type(call.onExpression);
                    type != nullptr &&
                    (unqualify(type->name) == Name{"Geometry"} ||
                    unqualify(type->name) == Name{"Topology"}))
                {

                    new_parameters.emplace_back(
                        VariableDeclaration {
                          Variable { 
                            call.call.return_type.with(Global()).with(Pointer()),
                            unqualify(call.call.name)
                          }
                        }
                    );

                    return make_printable(ArraySubscript{
                        make_printable(Variable{
                            call.call.return_type,
                            unqualify(call.call.name) }),
                        make_printable(GetIndex())});
                }
                return make_printable(call);
              })).visit(function.body);

              function.parameters.insert(
                function.parameters.end(),
                new_parameters.begin(),
                new_parameters.end()
              );

              for(const auto& param : new_parameters) {
                call.arguments.emplace_back(
                  make_printable(NotHandled { make_printable(param.variable) })
                );
                member_names.emplace_back(param.variable.name.name);
              }

              return call;
            }
            
            
    };
  }

  auto FixUpperFunctions() {
    return DepthFirstTransform {
      [](Function&& function) {
        
        if(!function.body) return function;

        std::vector<VariableDeclaration> parameters;

        function.body = MakeTransformUntil<Function>(
          printable_case(
            [&parameters](NotHandled&& not_handled) {
              
              auto argument = printable_get<Variable>(not_handled.wraps, "FixUpperFunctions: Not a variable");
              parameters.emplace_back(VariableDeclaration { argument });

              return not_handled.wraps;
            }
          )
        ).visit(function.body);

        function.parameters.insert(
          function.parameters.end(),
          parameters.begin(),
          parameters.end()
        );

        return function;

      }
    };
  }



} // namespace ct

#endif /* EXTRACTMEMBERCALLS_HPP */
