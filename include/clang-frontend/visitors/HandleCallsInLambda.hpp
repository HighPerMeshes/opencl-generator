#ifndef HANDLECALLSINLAMBDA_HPP
#define HANDLECALLSINLAMBDA_HPP

#include <clang-frontend.hpp>
#include <clang-frontend/visitors/HandleExecuteArgument.hpp>

#include <clang/Tooling/Tooling.h>

#include <clang-frontend/translator/Translator.hpp>
#include <unordered_set>

#include <util.hpp>

#include <clang-frontend/transformations/HandleKernel.hpp>
#include <clang-frontend/transformations/TranslateGet.hpp>
#include <clang-frontend/transformations/TranslateLocalBufferDecls.hpp>
#include <clang-frontend/transformations/Vec2FloatNTransform.hpp>
#include <clang-frontend/transformations/AddDeclsToKernel.hpp>
#include <clang-frontend/transformations/GetDofs.hpp>
#include <clang-frontend/transformations/RemoveRedundantConstructionExpressions.hpp>
#include <clang-frontend/transformations/ExtractMemberCalls.hpp>
#include <clang-frontend/transformations/InlineFunctions.hpp>
#include <clang-frontend/transformations/TranslateFreeLoops.hpp>
#include <clang-frontend/transformations/UnqualifyNames.hpp>
#include <clang-frontend/transformations/PrintFunctions.hpp>
#include <clang-frontend/transformations/MemberCallToFunctionCalls.hpp>
#include <clang-frontend/transformations/HandleOperatorCalls.hpp>
#include <clang-frontend/transformations/FunctionNames.hpp>
#include <clang-frontend/transformations/RemoveReferences.hpp>
#include <clang-frontend/transformations/HandleNames.hpp>
#include <clang-frontend/transformations/PropagateGlobal.hpp>

namespace ct
{

  template <typename Decls, typename Names>
  auto get_necessary_decls(
      const Decls &global_decls,
      const Names &needed_names)
  {

    auto names{needed_names};

    std::vector<ct::VariableDeclaration> used_decls;
    size_t old_size = -1;
    size_t new_size = 0;

    std::vector<ct::VariableDeclaration> already_seen;

    while (old_size != new_size)
    {
      for (auto &decl : used_decls)
      {
        decl = getNames(names).visit(decl);
      }
      unique_vector(names);

      std::vector<ct::VariableDeclaration> new_decls;
      std::copy_if(
          global_decls.begin(),
          global_decls.end(),
          std::back_inserter(new_decls),
          [&names](const auto &decl) {
            return std::find(names.begin(), names.end(), decl.variable.name) != names.end();
          });
      unique_vector(new_decls);

      std::copy_if(
          new_decls.begin(),
          new_decls.end(),
          std::inserter(used_decls, used_decls.begin()),
          [&already_seen](const ct::VariableDeclaration &decl) {
            auto result = std::find(already_seen.begin(), already_seen.end(), decl);
            if (result != already_seen.end())
            {
              return false;
            }
            else
            {
              already_seen.push_back(decl);
              return true;
            }
          });

      old_size = new_size;
      new_size = used_decls.size();
    }

    return used_decls;
  }

  auto array2Raw =
      [](const VariableDeclaration &decl) {
        if (decl.variable.type.name.name != "std::array")
          return decl;

        std::vector<Printable> dimensions;

        auto type = decl.variable.type;

        while (type.name.name == "std::array")
        {
          dimensions.push_back(type.template_arguments.at(1));
          auto first_tmp = type.template_arguments.at(0);
          type = printable_get<Type>(first_tmp, "array2Raw: type not found");
        }

        return VariableDeclaration{
            Variable{
                type,
                decl.variable.name},
            decl.definition,
            std::move(dimensions)};
      };

  auto HandleConstructExpressionCasts()
  {
    return Transformer{
        printable_case([](Cast &&cast) {
          if (auto *construct_expr = printable_get_optional<ConstructExpression>(cast.onExpression); construct_expr != nullptr)
          {
            return make_printable(*construct_expr);
          }
          else
          {
            return make_printable(cast);
          }
        })};
  }

  auto ArrayDeclarationsToPointer()
  {
    return Transformer{
        printable_case([](VariableDeclaration &&array_decl) {
          if (not array_decl.dimensions.empty() && array_decl.definition)
          {
            if (auto *construct_expr = printable_get_optional<ConstructExpression>(array_decl.definition.value()); construct_expr == nullptr)
            {
              array_decl.dimensions = std::vector<Printable>{};
              array_decl.variable.type = array_decl.variable.type.with(Pointer());
              return array_decl;
            }
          }
          return array_decl;
        })};
  }

  auto HandleDeclarations()
  {
    return Transformer{
        printable_case(
            [](VariableDeclaration &&array_decl) {
              if (array_decl.definition)
              {
                if (auto *initList = printable_get_optional<ConstructExpression>(array_decl.definition.value()); initList != nullptr)
                {

                  auto &arguments = initList->arguments;

                  if (arguments.empty())
                  {
                    array_decl.definition = std::nullopt;
                  }
                  if (arguments.size() == 1)
                  {
                    array_decl.definition = std::optional{arguments[0]};
                  }
                }
              }

              return array_decl;
            })};
  }

  auto HandleCopyConstruction()
  {
    return Transformer{
        printable_case([](ConstructExpression &&expr) {
          if (expr.arguments.size() != 1)
            return make_printable(expr);
          if (auto *arg_type = get_type(expr.arguments[0]); arg_type != nullptr && *arg_type == expr.constructs)
          {
            return make_printable(expr.arguments[0]);
          }
          return make_printable(expr);
        })};
  }

  auto getNames(std::vector<Name> &variableNames)
  {
    return Transformer{
        printable_case(
            [&variableNames](Variable &&variable) {
              variableNames.push_back(variable.name);
              return variable;
            })};
  }

  auto NamedCastToCast()
  {
    return Transformer{
        printable_case(
            [](NamedCast &&cast) {
              return Cast{
                  cast.castTo,
                  cast.onExpression};
            })};
  }

  auto HandleOmittedNames()
  {
    return Transformer{
        [i = 0](Name &&name) mutable {
          if (name.name.empty())
          {
            name.name = "omitted_";
            name.name += std::to_string(i);
            i++;
          }
          return name;
        }

    };
  }

  auto LinearizeMatrixAccess()
  {
    return Transformer{
        [](ArraySubscript &&subscript) {
          const auto *inner_subscript = printable_get_optional<ArraySubscript>(subscript.onExpression);
          if (!inner_subscript)
            return subscript;

          auto *on_type = get_type(inner_subscript->onExpression);
          if (!on_type)
            return subscript;
          if (on_type->name.name != "HPM::dataType::Matrix")
            return subscript;

          auto linear = BinaryOperator{inner_subscript->index, "+", make_printable(BinaryOperator{subscript.index, "*", on_type->template_arguments[1]})};

          return ArraySubscript{inner_subscript->onExpression, make_printable(linear)};
        }};
  }

  auto TranslateFunction(Environment &e, std::vector<Name> &used_variabe_names, std::vector<std::string> &member_names, std::set<size_t> &needed_vectors, std::set<std::pair<size_t, size_t>> &needed_matrices, Function &&k) -> Function
  {

    k.return_type = k.return_type.with(Kernel());

    return visit_all(
        k,
        HandleDofs(),
        HandlerOperatorFunctions(),
        HandlerOperatorMethods(),
        HandleCopyConstruction(),
        TranslateForEach(),
        TranslateGet(),
        TranslateLocalBufferDecls(),
        Vec2Vec(needed_vectors),
        Mat2Mat(needed_matrices),
        VectorCreation(),
        FixVecSubscripts(),
        RemoveRedundantConstructionExpressions(),
        RemoveTemplates(),
        Transformer{overloaded{printable_case(array2Raw), array2Raw}},
        HandleConstructExpressionCasts(),
        ArrayDeclarationsToPointer(),
        HandleDeclarations(),
        RemoveSimplices(),
        ReplaceLocalViews(),
        ExtractMemberCalls(member_names),
        FixUpperFunctions(),
        MemberCallToFunctionCalls(),
        FunctionNames(),
        FunctionCallNames(),
        getNames(used_variabe_names),
        HandleNames(),
        FixReferences(),
        RemoveReferences(),
        PropagateGlobal(),
        RemoveWrongGlobals(),
        NamedCastToCast(),
        ZeroInit(),
        HandleOmittedNames());
  }

} // namespace ct

#endif /* HANDLECALLSINLAMBDA_HPP */
