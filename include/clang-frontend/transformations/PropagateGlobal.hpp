#ifndef PROPAGATEGLOBAL_HPP
#define PROPAGATEGLOBAL_HPP

#include <ir.hpp>

namespace ct
{

  auto PropagateGlobal() {
    
    return Transformer {
      
      [](Function&& function) {

        std::vector<Name> globals;
        auto finder = MakeTransformUntil<Function>( [&globals](VariableDeclaration&& decl) {
          
          if(decl.variable.type.is(Global())) {
            globals.push_back(decl.variable.name);
          }

          return decl;
        } );
        
        function.body = finder.visit(function.body);
        function.parameters = finder.visit(function.parameters);

        function.body = MakeTransformUntil<Function>( [&globals](Variable&& variable) {
          
          if(std::find(globals.begin(), globals.end(), variable.name) != globals.end() && !variable.type.is(Global())) {
            variable.type = variable.type.with(Global());
          }

          return variable;
        } ).visit(function.body);

        function.body = MakeTransformUntil<Function>( [](FunctionCall&& call) {
          
          if(!call.function) return call;

          for(size_t i = 0; i < call.arguments.size(); ++i) {

            auto * arg_type = get_type(call.arguments[i]);
            auto * param_type = get_type(call.function.value().parameters[i]); 

            if(!arg_type || !param_type) {
              continue;
            }

            if(arg_type->is(Global()) && !param_type->is(Global())) {
              call.function.value().parameters[i].variable.type.qualifiers.insert(
                call.function.value().parameters[i].variable.type.qualifiers.begin(),
                Global()
              );
            }

          }

          return call;


        } ).visit(function.body);

        

        

        return function;

      }

    };
  }

  auto RemoveWrongGlobals() {
    return Transformer {
      [](Type&& type) {
        if(type.is(Global()) && !type.is(Pointer())) return type.without(Global());
        return type;
      }
    };
  }


} // namespace ct


#endif /* PROPAGATEGLOBAL_HPP */
