#ifndef TRANSLATEGET_HPP
#define TRANSLATEGET_HPP

#include <ir.hpp>

#include <sstream>

namespace ct
{

  FunctionCall GetGlobalIndex()
  {
    return {
        Int(),
        Name{"get_global_id"},
        {make_printable(Literal{Int(), "0"})},
        {}};
  }

  FunctionCall GetLocalSize() {
    return {
        Int(),
        Name{"get_local_size"},
        {make_printable(Literal{Int(), "0"})},
        {}};
  }

  FunctionCall GetLocalIndex() {
    return {
        Int(),
        Name{"get_local_id"},
        {make_printable(Literal{Int(), "0"})},
        {}};
  }

  auto GetIndex() {
    return GetGlobalIndex(); // Parenthesis { make_printable(BinaryOperator { make_printable(BinaryOperator{make_printable(GetGlobalIndex()), "*", make_printable(GetLocalSize())}), "+", make_printable(GetLocalIndex()) }) };
  }

  auto TranslateGet()
  {
    return Transformer{
        printable_case(
            [](FunctionCall &&call) {
              if (call.name.name != "std::get")
                return make_printable(call);

              auto index = call.template_arguments[0];

              std::stringstream ss;
              print(ss, index);
              size_t index_value = std::stoi(ss.str());

              const auto &arg = call.arguments[0];
              const auto &var = printable_get<Variable>(arg, "TranslateGet: variable not found");
              const auto &local_view_type = printable_get<Type>(var.type.template_arguments[index_value], "TranslateGet: Type of local view not found");

              const auto &dim_list = printable_get<Type>(var.type.template_arguments.back(), "TranslateGet: No DimensionalityList");

              std::stringstream dim_stream;
              print(dim_stream, dim_list.template_arguments[index_value]);
              size_t dim = std::stoi(dim_stream.str());

              const auto &buffer_type = printable_get<Type>(local_view_type.template_arguments.at(0), "TransalteGet: No Buffer Type");
              const auto &dof_type = printable_get<Type>(buffer_type.template_arguments.at(2), "TransalteGet: No Dof Type");

              auto lvs_var = make_printable<Variable>(
                  local_view_type,
                  Name{var.name.name + "_" + ss.str()});

              auto stride = make_printable(BinaryOperator {
                make_printable(GetIndex()),
                "*",
                dof_type.template_arguments[dim+1]
              });

              auto offset = make_printable(Variable{
                          Type{Name{"unsigned long"}},
                          Name{"hpm_offset_" + std::to_string(index_value)}});

              return make_printable(BinaryOperator {
                lvs_var, "+", make_printable(BinaryOperator { stride, "+", offset })
              });
            })};
  }

} // namespace ct

#endif /* TRANSLATEGET_HPP */
