#ifndef HANDLEOPERATORCALLS_HPP
#define HANDLEOPERATORCALLS_HPP

#include <ir.hpp>
#include <clang-frontend/transformations/UnqualifyNames.hpp>

namespace ct
{

    auto HandlerOperatorMethods()
    {

        auto member_call_lambda =
            [](MemberCall &&call) {
                if (unqualify(call.call.name) == Name{"operator[]"})
                {
                    return make_printable(
                        ArraySubscript{
                            call.onExpression,
                            call.call.arguments[0]});
                }

                if(unqualify(call.call.name) == Name{"operator="}) {
                    return make_printable(
                        Statement {
                            make_printable(
                                BinaryOperator {
                                    call.onExpression,
                                    "=",
                                    call.call.arguments[0]
                            })
                        }
                    );
                }

                return make_printable(call);
            };

        return Transformer{
            printable_case(member_call_lambda),
        };
    }

    auto HandlerOperatorFunctions()
    {

        auto make = [](auto&& call, std::string op){
            return make_printable(
                Statement {
                    make_printable(
                        BinaryOperator {
                            call.arguments[0],
                            op,
                            call.arguments[1]
                        }
                    )
                }
            );
        };

        auto function_call_lambda =
            [make](FunctionCall &&call) {
                if (unqualify(call.name) == Name{"operator[]"})
                {
                    return make_printable(
                        ArraySubscript{
                            call.arguments[0],
                            call.arguments[1]});
                }

                //Vector special treatment
                if (
                    call.function &&
                    call.function.value().parameters.size() == 2 &&
                    call.function.value().parameters[0].variable.type.name == Name { "Vec" } &&
                    call.function.value().parameters[0].variable.type.name == call.function.value().parameters[1].variable.type.name
                ) {

                    if(unqualify(call.name) == Name{"operator+="}) {
                        return make(call, "+=");
                    }
                    if(unqualify(call.name) == Name{"operator-="}) {
                        return make(call, "-=");
                    }
                    if(unqualify(call.name) == Name{"operator*="}) {
                        return make(call, "*=");
                    }
                    if(unqualify(call.name) == Name{"operator/="}) {
                        return make(call, "/=");
                    }

                    if(unqualify(call.name) == Name{"operator+"}) {
                        return make(call, "+");
                    }
                    if(unqualify(call.name) == Name{"operator-"}) {
                        return make(call, "-");
                    }
                    if(unqualify(call.name) == Name{"operator*"}) {
                        return make(call, "*");
                    }
                    if(unqualify(call.name) == Name{"operator/"}) {
                        return make(call, "/");
                    }



                }


                return make_printable(call);
            };

        return Transformer{
            printable_case(function_call_lambda),
        };
    }

} // namespace ct
#endif /* HANDLEOPERATORCALLS_HPP */
