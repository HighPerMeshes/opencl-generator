#ifndef MEMBERCALLTOFUNCTIONCALLS_HPP
#define MEMBERCALLTOFUNCTIONCALLS_HPP

#include <ir.hpp>

namespace ct
{
    
    auto MemberCallToFunctionCalls() {

        auto member_call_lambda = 
            [](MemberCall&& member_call) {
                
                
                auto * objectType = get_type(member_call.onExpression);
                
                if(objectType && objectType->name != Name { "auto"} ) {
                    
                    member_call.call.arguments.insert(
                        member_call.call.arguments.begin(),
                        make_printable(AddressOf(member_call.onExpression))
                    );

                }

                if(!member_call.call.function) {
                    return member_call.call;
                }

                auto & function = member_call.call.function.value();
                

                if(objectType && objectType->name != Name { "auto"} ) {
                    
                    function.parameters.insert(
                        function.parameters.begin(),
                        VariableDeclaration { Variable { objectType->with(Pointer()), "ocl_code_trafo_this" } }
                    );
                    
                }

                auto replace_this_ptrs = Transformer {
                    printable_case(
                        [](ThisPtr&& this_ptr) {
                            return Variable { this_ptr.type.with(Pointer()), "ocl_code_trafo_this" };       
                        }
                    )
                };
                  
                auto call = visit_all(member_call.call, replace_this_ptrs);

                return call;
            };

        return Transformer {
            printable_case(member_call_lambda)
        };
    }

} // namespace ct



#endif /* MEMBERCALLTOFUNCTIONCALLS_HPP */
