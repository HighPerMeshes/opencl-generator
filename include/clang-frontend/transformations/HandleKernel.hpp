#ifndef HANDLEKERNEL_HPP
#define HANDLEKERNEL_HPP

#include <ir.hpp>

#include <vector>

#include <clang-frontend/transformations/UnqualifyNames.hpp>

namespace ct
{

  // const VariableDeclaration mesh_info() { return {Variable{Type{Name{"int"}, {}, {Global(), Const(), Pointer()}}, Name{"mesh_info"}}}; }
  // const VariableDeclaration mesh_info_size() { return {Variable{Type{Name{"int"}}, Name{"mesh_info_size"}}}; }

  auto RemoveSimplices()
  {

    return Transformer{
        overloaded{
            [](FunctionCall &&call) {
              std::vector<Printable> new_arguments;
              for (const auto &argument : call.arguments)
              {

                auto *type = get_type(argument);
                if (type && type->name == Name{"HPM::entity::Simplex"})
                {
                }
                else
                {
                  new_arguments.emplace_back(argument);
                }
              }

              call.arguments = new_arguments;

              return call;
            },
            [](Function &&function) {
              std::vector<VariableDeclaration> new_parameters;
              for (const auto &parameter : function.parameters)
              {

                auto *type = get_type(parameter);
                if (type && type->name == Name{"HPM::entity::Simplex"})
                {
                }
                else
                {
                  new_parameters.emplace_back(parameter);
                }
              }

              function.parameters = new_parameters;

              return function;
            }}};
  }

  auto ReplaceLocalViews()
  {

    return Transformer{
        overloaded{
            [](FunctionCall &&call) {
              std::vector<Printable> new_arguments;
              for (const auto &argument : call.arguments)
              {

                auto *type = get_type(argument);
                if (type && type->name == Name{"std::tuple"} && !type->template_arguments.empty())
                {

                  size_t local_view_num{0};
                  for (auto &tuple_template_argument : type->template_arguments)
                  {

                    auto &local_buffer_type = printable_get<Type>(tuple_template_argument, "HandleKernel: local_buffer_type not found");

                    if(local_buffer_type.name.name == "HPM::internal::LocalView::DimensionalityList") continue;
                    
                    auto &buffer_type = printable_get<Type>(local_buffer_type.template_arguments[0], "HandleKernel: buffer_type not found");
                    auto &buffer_data_type = printable_get<Type>(buffer_type.template_arguments[0], "HandleKernel: buffer_data_type not found");

                    auto &variable = printable_get<Variable>(argument, "HandleKernel: Not a variable");

                    new_arguments.emplace_back(
                        make_printable(Variable{
                            buffer_data_type.with(Global()).with(Pointer()),
                            Name{variable.name.name + "_" + std::to_string(local_view_num)}}));

                    ++local_view_num;
                  }

                  for (size_t offset_num = 0; offset_num < local_view_num; ++offset_num)
                  {
                    new_arguments.emplace_back(
                        make_printable(Variable{
                            Type{Name{"unsigned long"}},
                            Name{"hpm_offset_" + std::to_string(offset_num)}}));
                  }

                }
                else
                {
                  new_arguments.emplace_back(argument);
                }
              }

              call.arguments = new_arguments;

              return call;
            },
            [](Function &&function) {
              std::vector<VariableDeclaration> new_parameters;
              for (const auto &parameter : function.parameters)
              {

                auto *type = get_type(parameter);
                if (type && type->name == Name{"std::tuple"} && !type->template_arguments.empty())
                {

                  size_t local_view_num{0};
                  for (auto &tuple_template_argument : type->template_arguments)
                  {

                    auto &local_buffer_type = printable_get<Type>(tuple_template_argument, "HandleKernel: template_type not found");
                    
                    if(local_buffer_type.name.name == "HPM::internal::LocalView::DimensionalityList") continue;
                    
                    auto &buffer_type = printable_get<Type>(local_buffer_type.template_arguments[0], "HandleKernel: buffer_type not found");
                    auto &buffer_data_type = printable_get<Type>(buffer_type.template_arguments[0], "HandleKernel: buffer_data_type not found");

                    new_parameters.emplace_back(
                        VariableDeclaration{
                            buffer_data_type.with(Global()).with(Pointer()),
                            Name{parameter.variable.name.name + "_" + std::to_string(local_view_num)}});

                    ++local_view_num;
                  }

                  for (size_t offset_num = 0; offset_num < local_view_num; ++offset_num)
                  {
                    new_parameters.emplace_back(
                        VariableDeclaration{
                            Type{Name{"unsigned long"}},
                            Name{"hpm_offset_" + std::to_string(offset_num)}});
                  }
                }
                else
                {
                  new_parameters.emplace_back(parameter);
                }
              }

              function.parameters = new_parameters;

              return function;
            }

        }};
  } // namespace ct
} // namespace ct

#endif /* HANDLEKERNEL_HPP */
