#ifndef PRINTFUNCTIONS_HPP
#define PRINTFUNCTIONS_HPP

#include <ir.hpp>

namespace ct
{
    template<typename Stream>
    auto PrintFunctions(Stream& stream) {
        
        auto lambda = [&stream](Function&& function) {
            print(stream, function, "\n\n");
            return function;
        };
        
        return DepthFirstTransform {
            overloaded {
                printable_case(lambda),
                lambda
            }
        };
    }

} // namespace ct


#endif /* PRINTFUNCTIONS_HPP */
