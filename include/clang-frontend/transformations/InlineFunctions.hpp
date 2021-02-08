#ifndef INLINEFUNCTIONS_HPP
#define INLINEFUNCTIONS_HPP

#include <ir.hpp>
#include <util.hpp>
#include <map>

namespace ct
{

    // auto get_map(const std::vector<Printable> &arguments, const std::vector<VariableDeclaration> &parameters)
    // {

    //     if (arguments.size() != parameters.size())
    //     {
    //         fail_with_message("get_map: arguments.size != parameters.size");
    //     }

    //     std::map<std::string, Printable> map;
    //     for (
    //         size_t i = 0;
    //         i < arguments.size();
    //         ++i)
    //     {
    //         map.emplace(parameters[i].variable.name.name, arguments[i]);
    //     }

    //     for (const auto &[k, v] : map)
    //     {
    //         print(std::cout, k, ": ", v, "\n");
    //     }

    //     return map;
    // }

    // auto handle_inlining(const std::map<std::string, Printable> &name_to_replacement, Block &&function_body)
    // {

    //     auto transformation = Transformer{
    //         printable_case([&name_to_replacement](Variable &&var) {
    //             if (name_to_replacement.find(var.name.name) == name_to_replacement.end())
    //             {
    //                 return make_printable(var);
    //             }
    //             return make_printable(name_to_replacement.at(var.name.name));
    //         })};

    //     return transformation.visit_until<Function>(std::move(function_body));
    // }

    // auto inline_functions_impl(FunctionCall &&function_call)
    // {

    //     if (!function_call.function)
    //     {
    //         return make_printable(function_call);
    //     }
    //     if (!function_call.function.value().body)
    //     {
    //         return make_printable(function_call);
    //     }
    //     if (function_call.function.value().return_type.name.name != "void")
    //     {
    //         return make_printable(function_call);
    //     }

    //     auto function = function_call.function.value();
    //     auto arguments = function_call.arguments;

    //     auto map = get_map(arguments, function.parameters);

    //     return make_printable(handle_inlining(map, std::move(function_call.function.value().body.value())));
    // }

    // auto InlineFunctions()
    // {

    //     return Transformer{
    //         printable_case(
    //             [](FunctionCall &&function_call) {
    //                 return inline_functions_impl(std::move(function_call));
    //             })};
    // }

} // namespace ct

#endif /* INLINEFUNCTIONS_HPP */
