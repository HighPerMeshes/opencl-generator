#ifndef REMOVEREFERENCES_HPP
#define REMOVEREFERENCES_HPP

#include <ir.hpp>

namespace ct
{

  auto RemoveReferences() {
    return Transformer {
      [](Type&& type) {
        return type.without(Reference());
      }
    };
  }

  auto FixReferences()
  {
    return Transformer{
        [](FunctionCall &&c) {
          
          auto call = c;

          if(!call.function) return call;

          auto& function = call.function.value();
          
          auto fixer = [](const auto& param) { return MakeTransformUntilDepthFirst<Function>(
                  printable_case([&param](Variable &&variable) {
                    
                    if (variable.name != param.variable.name) {
                      return make_printable(variable);
                    }
                      
                    variable.type = variable.type.with(Pointer());

                    return make_printable(Parenthesis { make_printable(UnaryOperator { "*", make_printable(variable) }) });

                  }));

              };

          size_t i = 0;
          for (auto &param : function.parameters)
          {

            if (param.variable.type.is(Const()) || !param.variable.type.is(Reference()))
            {
              param.variable.type = param.variable.type.without(Reference());
            }
            else
            {
              param.variable.type = param.variable.type.without(Reference()).with(Pointer());

              function.body = visit_all(function.body, fixer(param));

              call.arguments[i] = make_printable(AddressOf { call.arguments[i] });
              
            }

            ++i;
          }

          return call;
        }};
  }

  auto RemoveTemplates()
  {
    return Transformer{
        [&](FunctionCall &&call) {
          call.template_arguments = std::vector<Printable>{};
          return call;
        }};
  }

} // namespace ct

#endif /* REMOVEREFERENCES_HPP */
