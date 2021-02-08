#ifndef TRANSLATEFREELOOPS_HPP
#define TRANSLATEFREELOOPS_HPP

#include <ir.hpp>

namespace ct
{
    
auto TranslateForEach() {
    return Transformer {
        printable_case(
            [](FunctionCall&& call) {
                
                if(call.name.name != "HPM::ForEach") {
                    return make_printable(std::move(call));
                }
                
                auto& lambda = printable_get<Lambda>(call.arguments[1], "2nd argument of HPM::ForEach is not a lambda.");
                auto iteration_variable = std::move(lambda.parameters[0]);
                auto& upper_limit = call.arguments[0];

                iteration_variable.variable.type = iteration_variable.variable.type.without(Const());

                iteration_variable.definition = 
                    make_printable<Literal>(
                        iteration_variable.variable.type,
                        "0"
                    );

                InitConditionIncrement init {
                    make_printable(iteration_variable),
                    make_printable(BinaryOperator { make_printable(iteration_variable.variable), "<" , std::move(upper_limit) }),
                    make_printable(UnaryOperator { "++", make_printable(iteration_variable.variable) })
                };


                return make_printable(
                    ForLoop {
                        make_printable(std::move(init)),
                        std::move(lambda.body)
                    }
                );
            }
        )
    };
}

} // namespace ct


#endif /* TRANSLATEFREELOOPS_HPP */
