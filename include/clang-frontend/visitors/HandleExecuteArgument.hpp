#ifndef HANDLEEXECUTEARGUMENT_HPP
#define HANDLEEXECUTEARGUMENT_HPP

#include <clang/Tooling/Tooling.h>
#include <clang-frontend/visitors/HandleCallsInLambda.hpp>
#include <clang-frontend/visitors/HandleLoopBody.hpp>

#include <util.hpp>

#include <fstream>

namespace ct
{

  struct HandleExecuteArgument : public Visitor<HandleExecuteArgument>
  {

    std::fstream& output;
    std::vector<std::string> member_names;
    std::string kernel_name;

    bool VisitCXXRecordDecl(clang::CXXRecordDecl *d)
    {
      using namespace clang;

      auto *decl = dyn_cast<ClassTemplateSpecializationDecl>(d);

      if (!decl)
      {
        d->dumpColor();
        fail_with_message("decl not a template specialization");
      }

      const auto &arguments = decl->getTemplateArgs();

      if (arguments.size() < 3)
      {
        decl->dumpColor();
        fail_with_message("less than 3 arguments");
      }

      auto *recordDecl = arguments.get(3).getAsType()->getAsCXXRecordDecl();

      if (!recordDecl)
      {
        decl->dumpColor();
        fail_with_message("not a record decl");
      }

      std::vector<Name> used_variable_names;

      HandleLoopBody handler{environment};
      handler.TraverseCXXRecordDecl(recordDecl);

      Function& kernel = handler.kernel;

      std::set<size_t> needed_vectors;
      std::set<std::pair<size_t, size_t>> needed_matrices; 

      kernel = TranslateFunction(environment, used_variable_names, member_names, needed_vectors, needed_matrices, std::move(kernel));
      
      unique_vector(used_variable_names);
      unique_vector(member_names);

      auto constexpr_decls = handler.translator.constexpr_decls;
      unique_vector(constexpr_decls);

      auto used_decls = get_necessary_decls(constexpr_decls, used_variable_names);

      kernel = Transformer{
        [&](VariableDeclaration &&decl) {
          if (!decl.definition)
            return decl;

          bool found = false;
          decl = Transformer{
              [&](Name &&name) {
                found = found || std::find_if(used_decls.begin(), used_decls.end(), [&](const auto &decl) { return name == decl.variable.name; }) != used_decls.end();
                return name;
              }}.visit(std::move(decl));

          if (found)
          {
            decl.variable.type = decl.variable.type.without(Pointer()).with({Constant(), Pointer()});
          }

          return decl;
        }}.visit(std::move(kernel));

      for (auto &decl : used_decls)
      {
        decl = array2Raw(std::move(decl));
        decl = visit_all(
            decl,
            HandleConstructExpressionCasts(),
            RemoveRedundantConstructionExpressions(),
            HandleNames(),
            Vec2Vec(needed_vectors),
            Transformer{
                printable_case([](ConstructExpression &&expression) {
                  if (expression.arguments.size() != 1)
                    return make_printable(expression);
                  return expression.arguments[0];
                })},
            Transformer{
                printable_case([](ConstructExpression &&expression) {
                  std::string name = expression.constructs.name.name;
                  std::smatch sm;
                  std::regex e{"[0-9]+"};
                  bool found = std::regex_search(name, sm, e);
                  if (!found)
                    return expression;
                  size_t size = std::stoi(sm.str());

                  for (size_t additional = 0; additional < size - expression.arguments.size(); ++additional)
                  {
                    expression.arguments.emplace_back(
                        make_printable(Literal{Float(), "0"}));
                  }

                  return expression;
                })});

        decl.variable.type = decl.variable.type.with(Constant());
      }

      for(auto size : needed_vectors) {
        print(output, "typedef struct Vector_double_", size, "{\n double values[", size, "];\n} Vector_double_", size, ";\n");
      }

      for(auto [x, y] : needed_matrices) {
        print(output, "typedef struct Matrix_double_", x, "_", y,  "{\n Vector_double_", x, " values[", y ,"];\n} Matrix_double_", x, "_", y, ";\n");
      }

      for(auto& decl : used_decls) {
        print(output, decl, ";\n\n");
      }

      kernel = visit_all(kernel, PrintFunctions(output));

      kernel_name = kernel.name.name;

      return true;
    }
  };

} // namespace ct

#endif /* HANDLEEXECUTEARGUMENT_HPP */
