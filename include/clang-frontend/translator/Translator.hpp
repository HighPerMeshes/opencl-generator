#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include <clang/Tooling/Tooling.h>

#include <ir.hpp>
#include <clang-frontend.hpp>
#include <util/flat_vector.hpp>

namespace ct
{

  const auto &GetTemplates(const clang::CallExpr *e)
  {
    return *e->getDirectCallee()->getTemplateSpecializationArgs();
  }

  const auto &GetTemplates(const clang::CXXRecordDecl *d)
  {

    return clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(d)->getTemplateArgs();
  }

  bool HasTemplates(const clang::CXXRecordDecl *d)
  {
    return clang::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(d) ? true : false;
  }

  struct Translator
  {

    Environment &environment;
    std::vector<VariableDeclaration> constexpr_decls;

    Type MakeType(const clang::QualType &qualType, bool global = false)
    {
      auto *lvalueRef = clang::dyn_cast_or_null<clang::LValueReferenceType>(qualType.getTypePtr());
      auto *rvalueRef = clang::dyn_cast_or_null<clang::RValueReferenceType>(qualType.getTypePtr());

      auto type = lvalueRef ? lvalueRef->getPointeeType() : rvalueRef ? rvalueRef->getPointeeType() : qualType;

      type = type.getDesugaredType(environment.context);

      auto getQualifiers = [&type, lvalueRef, rvalueRef]() {
        std::vector<Qualifier> res;

        if (type.isConstQualified())
        {
          res.emplace_back(Const());
        }
        if (lvalueRef)
        {
          res.emplace_back(Reference());
        }
        if (rvalueRef)
        {
          res.emplace_back(RValue());
        }
        if (type->isPointerType())
        {
          res.emplace_back(Pointer());
        }

        return res;
      };

      if (type->isEnumeralType())
      {
        auto enum_type = clang::dyn_cast<clang::EnumType>(type);
        return {
            Name{enum_type->getDecl()->getQualifiedNameAsString()}, {}, getQualifiers()};
      }

      if (type->isPointerType())
      {
        auto ptr_type = clang::dyn_cast<clang::PointerType>(type);
        auto deduced_type = MakeType(ptr_type->getPointeeType());
        return deduced_type.with(Pointer());
      }

      if (type->isBuiltinType())
      {

        return {Name{TypeAsString(type)},
                {},
                getQualifiers()};
      }

      auto *recordDecl = type->getAsCXXRecordDecl();
      if (recordDecl)
      {

        const auto *templates = HasTemplates(recordDecl) ? &GetTemplates(recordDecl) : nullptr;

        return {Name{recordDecl->isLambda() ? "auto" : recordDecl->getQualifiedNameAsString()},
                templates ? handle(*templates) : std::vector<Printable>{},
                getQualifiers()};
      }

      const auto *constantArray = clang::dyn_cast<clang::ConstantArrayType>(type);
      if (constantArray)
      {

        return {
            Name{"std::array"},
            std::vector{
                make_printable(MakeType(constantArray->getElementType())),
                make_printable(Literal{SizeT(), constantArray->getSize().toString(10, false)})},
            getQualifiers()};
      }

      const auto *lvalue_ref_type = clang::dyn_cast<clang::LValueReferenceType>(type);
      if (lvalue_ref_type)
      {
        return MakeType(lvalue_ref_type->desugar());
      }

      const auto *rvalue_ref_type = clang::dyn_cast<clang::RValueReferenceType>(type);
      if (rvalue_ref_type)
      {
        return MakeType(rvalue_ref_type->desugar());
      }

      if (const auto *template_type = clang::dyn_cast<clang::TemplateTypeParmType>(type); template_type != nullptr)
      {
        return {
            Name{"auto"},
            std::vector<Printable>{},
            getQualifiers()};
      }

      if (const auto *auto_type = clang::dyn_cast<clang::AutoType>(type); auto_type != nullptr)
      {
        return {
            Name{"auto"},
            std::vector<Printable>{},
            getQualifiers()};
      }

      horizontal_line();
      type.dump();
      fail_with_message("Could not translate the type given above");
    }

    template <typename CastTo, typename CastFrom>
    std::optional<Printable> match_once(const CastFrom &from)
    {
      if (
          auto *casted = llvm::dyn_cast_or_null<CastTo>(&from);
          casted != nullptr)
      {
        return {make_printable(handle(*casted))};
      }
      else
      {
        return {};
      }
    }

    template <typename CastFrom>
    std::optional<Printable> match(const CastFrom & /* from */)
    {
      return {};
    }

    template <typename FirstCastTo, typename... CastTos, typename CastFrom>
    std::optional<Printable> match(const CastFrom &from)
    {
      auto result = match_once<FirstCastTo>(from);
      return result ? result : match<CastTos...>(from);
    }

    Printable MatchStmt(const clang::Stmt &stmt)
    {
      auto result = match<
          clang::Expr, clang::DeclStmt, clang::ReturnStmt, clang::NullStmt, clang::IfStmt, clang::ForStmt, clang::CXXForRangeStmt>(stmt);

      if (!result)
      {
        horizontal_line();
        println("Can not deduce stmt\n");

        stmt.dumpColor();

        horizontal_line();
      }
      return result ? result.value() : make_printable<Unknown>(std::string{"Can not deduce stmt"});
    }

    If handle(const clang::IfStmt &if_stmt)
    {

      auto *cond = if_stmt.getCond();
      if (!cond)
      {
        fail_with_message("clang::IfStmt: No condition");
      }
      auto *then = if_stmt.getThen();
      if (!then)
      {
        fail_with_message("clang::IfStmt: No True Branch");
      }

      auto init = if_stmt.getInit() ? std::optional<Printable>{make_printable(handle(*if_stmt.getInit()))} : std::optional<Printable>{};
      auto condition = handle(*cond);
      auto true_branch = handleBody(*then);
      auto else_branch = if_stmt.hasElseStorage() ? handleBody(*if_stmt.getElse()) : Block{};

      return {
          init,
          condition,
          true_branch,
          else_branch};
    }

    ForLoop handle(const clang::ForStmt &stmt)
    {

      const auto *init = stmt.getInit();

      const auto *cond = stmt.getCond();

      const auto *inc = stmt.getInc();

      return ForLoop{
          make_printable(InitConditionIncrement{
              (init) ? handle(*init).wraps : make_printable(NullStmt{}),
              (cond) ? handle(*cond) : make_printable(NullStmt{}),
              (inc) ? handle(*inc) : make_printable(NullStmt{})}),
          body_of(stmt)};
    }

    ForLoop handle(const clang::CXXForRangeStmt &stmt)
    {

      const auto &loop_variable = deref(stmt.getLoopVariable(), "CXXForRangeStmt: No loop variable");
      const auto &range_init = deref(stmt.getRangeInit(), "CXXForRangeStmt: No range init");

      return ForLoop{
          make_printable(Range{
              handle(loop_variable),
              handle(range_init)}),
          body_of(stmt)};
    }

    ArraySubscript handle(const clang::ArraySubscriptExpr &expr)
    {
      const auto &lhs = deref(expr.getLHS(), "clang::ArraySubscriptExpr: No LHS");
      const auto &rhs = deref(expr.getRHS(), "clang::ArraySubscriptExpr: No RHS");

      return {
          handle(lhs),
          handle(rhs)};
    }

    Printable handle(const clang::ConstantExpr &expr)
    {
      return subexpr_of(expr);
    }

    ThisPtr handle(const clang::CXXThisExpr &expr)
    {
      return {type_of(expr)};
    }

    NullStmt handle(const clang::NullStmt &stmt)
    {
      return Null;
    }

    template <typename T>
    auto deref(const T *ptr, std::string failure_message) -> const T &
    {
      if (!ptr)
      {
        fail_with_message(failure_message);
      }
      return *ptr;
    }

    template <typename HasParameters>
    std::vector<VariableDeclaration> parameters_of(const HasParameters &has_parameters)
    {

      std::vector<VariableDeclaration> parameters;
      for (auto *param : has_parameters.parameters())
      {
        if (!param)
        {
          fail_with_message("parameters_of: param not valid");
        }
        parameters.emplace_back(handle(*param));
      }
      return parameters;
    }

    Block handleBody(const clang::Stmt &stmt)
    {
      std::vector<Statement> body;
      auto *compound_stmt = clang::dyn_cast_or_null<clang::CompoundStmt>(&stmt);

      if (!compound_stmt)
      {
        body.emplace_back(handle(stmt));
      }
      else
      {
        for (const auto *stmt : compound_stmt->body())
        {
          if (!stmt)
          {
            fail_with_message("handleBody: stmt not valid");
          }
          body.emplace_back(handle(*stmt));
        }
      }

      return {body};
    }

    template <typename HasBody>
    Block body_of(const HasBody &has_body)
    {

      if (!has_body.getBody())
      {
        has_body.dumpColor();
        fail_with_message("body_of: has_body has no body");
      }

      return handleBody(*has_body.getBody());
    }

    Function handle(const clang::CXXMethodDecl &method)
    {

      auto parameters = parameters_of(method);
      auto body = body_of(method);

      return {
          MakeType(method.getReturnType()),
          qualifiedName(method),
          std::move(parameters),
          std::move(body)};
    }

    Function handle(const clang::FunctionDecl &decl)
    {

      auto parameters = parameters_of(decl);
      auto body = decl.getBody() ? body_of(decl) : Block{};

      return {
          MakeType(decl.getReturnType()),
          qualifiedName(decl),
          std::move(parameters),
          std::move(body)};
    }

    template <typename HasSubExpr>
    Printable subexpr_of(const HasSubExpr &hasSubExpr)
    {
      auto *subExpr = hasSubExpr.getSubExpr();
      if (!subExpr)
      {
        fail_with_message("subexpr_of: No Sub Expr");
      }
      return handle(*subExpr);
    }

    Printable handle(const clang::ImplicitCastExpr &expr)
    {
      return subexpr_of(expr);
    }

    MemberAccess handle(const clang::CXXDependentScopeMemberExpr &expr)
    {
      const auto &base = deref(expr.getBase(), "CXXDependentScopeMemberExpr: No base");

      return {
          handle(base),
          Variable{
              type_of(expr),
              Name{expr.getMember().getAsString()}},
          expr.isArrow()};
    }

    MemberAccess handle(const clang::MemberExpr &expr)
    {

      const auto &base = deref(expr.getBase(), "MemberExpr: No base");
      const auto &member_decl = deref(expr.getMemberDecl(), "MemberExpr: No member decl");

      return {
          handle(base),
          Variable{
              type_of(member_decl),
              qualifiedName(member_decl)},
          expr.isArrow()};
    }

    Return handle(const clang::ReturnStmt &stmt)
    {
      return stmt.getRetValue() ? Return{handle(*stmt.getRetValue())} : Return{};
    }

    Lambda handle(const clang::LambdaExpr &expr)
    {

      std::vector<Lambda::Capture> captures;
      for (const auto &capture : expr.captures())
      {
        bool by_ref = capture.getCaptureKind() == clang::LambdaCaptureKind::LCK_ByRef;

        if (capture.capturesVariable())
        {
          const auto &captured_var = deref(capture.getCapturedVar(), "LambdaExpr: No captured var");

          captures.emplace_back(Lambda::Capture{make_printable(handle(captured_var).variable), by_ref});
        }
        if( capture.capturesThis())
        {
          captures.emplace_back(Lambda::Capture{ make_printable(ThisPtr{ Type { Name { "auto"}, {}, {} } }), by_ref});
        }

      }

      const auto &call_operator = deref(expr.getCallOperator(), "LambdaExpr: No call operator");
      auto parameters = parameters_of(call_operator);
      auto body = body_of(call_operator);

      auto defaultCapture = expr.getCaptureDefault();
      Lambda::DefaultCapture translated =
          defaultCapture == clang::LambdaCaptureDefault::LCD_None ? Lambda::none : defaultCapture == clang::LambdaCaptureDefault::LCD_ByRef ? Lambda::by_reference : Lambda::by_value;

      return {
          translated,
          std::move(captures),
          std::move(parameters),
          std::move(body)};
    }

    Literal handle(const clang::IntegerLiteral &literal)
    {
      return {
          type_of(literal),
          literal.getValue().toString(10, true)};
    }

    Literal handle(const clang::FloatingLiteral &literal)
    {

      
      auto result = environment.get_text(
        clang::SourceRange {
          literal.getBeginLoc(),
          literal.getEndLoc()
        }
      );

      return {
          type_of(literal),
          result};
    }

    Literal handle(const clang::StringLiteral &literal)
    {
      return {
          type_of(literal),
          std::string{literal.getString()}};
    }

    BinaryOperator handle(const clang::BinaryOperator &binary_op)
    {
      auto *lhs = binary_op.getLHS();
      if (!lhs)
      {
        fail_with_message("clang::BinaryOperator: No LHS");
      }

      auto *rhs = binary_op.getRHS();
      if (!rhs)
      {
        fail_with_message("clang::BinaryOperator: No RHS");
      }

      return {handle(*lhs), binary_op.getOpcodeStr().str(), handle(*rhs)};
    }

    template <typename Named>
    Name qualifiedName(const Named &named)
    {
      return {
          named.getQualifiedNameAsString()};
    }

    template <typename Typed>
    Type type_of(const Typed &typed)
    {
      return MakeType(typed.getType());
    }

    VariableDeclaration handle(const clang::VarDecl &decl)
    {

      Variable variable{MakeType(decl.getType()), qualifiedName(decl)};

      auto init = decl.hasInit() ? std::optional{make_printable(handle(*decl.getInit()))} : std::optional<Printable>{};

      return {std::move(variable), std::move(init)};
    }

    Variable handle(const clang::DeclRefExpr &expr)
    {

      auto *decl = expr.getDecl();
      if (!decl)
      {
        fail_with_message("DeclRefExpr does not have decl");
      }

      if (auto *var_decl = clang::dyn_cast<clang::VarDecl>(decl); var_decl != nullptr && var_decl->isConstexpr())
      {
        constexpr_decls.emplace_back(handle(*var_decl));
      }

      return {type_of(expr), qualifiedName(*decl)};
    }

    Printable handle(const clang::ExprWithCleanups &expr)
    {
      return subexpr_of(expr);
    }

    Printable handle(const clang::CXXStdInitializerListExpr &expr)
    {
      return subexpr_of(expr);
    }

    Printable handle(const clang::MaterializeTemporaryExpr &expr)
    {
      return subexpr_of(expr);
    }

    Printable handle(const clang::CXXBindTemporaryExpr &expr)
    {
      return subexpr_of(expr);
    }

    Printable handle(const clang::SubstNonTypeTemplateParmExpr &substitute)
    {

      auto *replacement = substitute.getReplacement();
      if (!replacement)
      {
        fail_with_message("SubstNonTypeTemplateParmExpr: No Replacement");
      }

      return handle(*replacement);
    }

    Cast handle(const clang::CXXFunctionalCastExpr &expr)
    {
      return {
          MakeType(expr.getTypeAsWritten()),
          subexpr_of(expr)};
    }

    NamedCast handle(const clang::CXXNamedCastExpr &expr)
    {
      return {
          Name{std::string{expr.getCastName()}},
          MakeType(expr.getTypeAsWritten()),
          subexpr_of(expr)};
    }

    Printable handle(const clang::Expr &expr)
    {

      auto result =
          match<
              clang::BinaryOperator,
              clang::ArraySubscriptExpr,
              clang::CXXStdInitializerListExpr,
              clang::CXXMemberCallExpr,
              clang::MaterializeTemporaryExpr,
              clang::CXXBindTemporaryExpr,
              clang::SubstNonTypeTemplateParmExpr,
              clang::DeclRefExpr,
              clang::CXXOperatorCallExpr,
              clang::IntegerLiteral,
              clang::FloatingLiteral,
              clang::StringLiteral,
              clang::CXXNamedCastExpr,
              clang::CXXFunctionalCastExpr,
              clang::CallExpr,
              clang::ParenExpr,
              clang::LambdaExpr,
              clang::MemberExpr,
              clang::CXXThisExpr,
              clang::CXXDependentScopeMemberExpr,
              clang::InitListExpr,
              clang::ImplicitCastExpr,
              clang::CXXConstructExpr,
              clang::ExprWithCleanups,
              clang::ConstantExpr,
              clang::UnaryOperator>(expr);

      if (!result)
      {
        horizontal_line();
        println("Can not deduce expr\n");

        expr.dumpColor();

        horizontal_line();
      }

      return result ? result.value() : make_printable<Unknown>("Could not deduce expr");
    }

    template <typename Range>
    std::vector<Printable> handleArguments(Range &&argument_range)
    {
      std::vector<Printable> res;
      for (const auto *arg : argument_range)
      {
        if (!arg)
        {
          fail_with_message("handleArguments: arg not valid");
        }
        res.emplace_back(make_printable(handle(*arg)));
      }
      return res;
    }

    template <typename HasArguments>
    std::vector<Printable> arguments_of(const HasArguments &has_arguments)
    {
      return handleArguments(has_arguments.arguments());
    }

    ConstructExpression handle(const clang::CXXConstructExpr &expr)
    {
      return {
          type_of(expr),
          arguments_of(expr),
          expr.isListInitialization()};
    }

    ConstructExpression handle(const clang::InitListExpr &expr)
    {
      return {
          type_of(expr),
          handleArguments(expr.inits()),
          true};
    }

    Printable handle(const clang::Decl &decl)
    {
      auto result = match<clang::VarDecl>(decl);
      return result ? std::move(result.value()) : make_printable<Unknown>("Can not deduce decl");
    }

    Parenthesis handle(const clang::ParenExpr &expr)
    {
      return {subexpr_of(expr)};
    }

    Printable handle(const clang::DeclStmt &decl)
    {
      if (!decl.isSingleDecl())
      {
        decl.dumpColor();
        fail_with_message("not a single declaration");
      }

      auto *singleDecl = decl.getSingleDecl();

      return handle(*singleDecl);
    }

    Statement handle(const clang::Stmt &stmt)
    {
      return Statement{MatchStmt(stmt)};
    }

    std::vector<Printable> handle(const clang::TemplateArgumentList &arg_list)
    {
      std::vector<Printable> res;
      for (const auto &arg : arg_list.asArray())
      {
        flat_vector_push_back(res, handle(arg));
      }
      return res;
    }

    std::vector<Printable> handle(const clang::TemplateArgument &arg)
    {
      switch (arg.getKind())
      {
      case clang::TemplateArgument::Type:
      {
        return {make_printable(MakeType(arg.getAsType()))};
      }
      case clang::TemplateArgument::Pack:
      {
        std::vector<Printable> res;
        for (const auto packEntry : arg.getPackAsArray())
        {
          flat_vector_push_back(res, handle(packEntry));
        }
        return res;
      }
      case clang::TemplateArgument::Integral:
      {
        return {make_printable<Literal>(Type{Name{"int64_t"}, {}, {}}, arg.getAsIntegral().toString(10))};
      }
      case clang::TemplateArgument::Template:
      {
        auto tmp = arg.getAsTemplate();
        return {make_printable<Name>(qualifiedName(*tmp.getAsTemplateDecl()))};
      }
      default:
        return {make_printable<Unknown>("could not deduce template argument")};
      }
    }

    FunctionCall callExprOnly(const clang::CallExpr &expr)
    {
      auto args = arguments_of(expr);

      std::vector<Printable> template_args;
      if (auto *callee = expr.getDirectCallee(); callee != nullptr)
      {
        std::optional<Function> function{handle(*callee)};

        auto name = qualifiedName(*callee);

        auto *templates = callee->getTemplateSpecializationArgs();

        if (templates)
        {

          for (auto template_arg : templates->asArray())
          {
            flat_vector_push_back(template_args, handle(template_arg));
          }
        }

        auto return_type = MakeType(expr.getCallReturnType(environment.context));

        return {std::move(return_type), Name{name}, std::move(args), std::move(template_args), std::move(function)};
      }
      if (auto *callee = clang::dyn_cast<clang::UnresolvedLookupExpr>(expr.getCallee()); callee != nullptr)
      {
        auto name = callee->getName().getAsString();

        for (auto template_arg : callee->template_arguments())
        {
          flat_vector_push_back(template_args, handle(template_arg.getArgument()));
        }

        auto return_type = MakeType(expr.getCallReturnType(environment.context));

        return {std::move(return_type), Name{name}, std::move(args), std::move(template_args)};
      }

      expr.dumpColor();
      fail_with_message("getDirectCallee in callExprOnly returned null");
    };

    MemberCall handle(const clang::CXXMemberCallExpr &memberCallExpr)
    {

      auto *implicit_object = memberCallExpr.getImplicitObjectArgument();
      if (!implicit_object)
      {
        fail_with_message("CXXMemberCallExpr: no implicit object");
      }

      return {
          handle(*implicit_object),
          callExprOnly(memberCallExpr)};
    }

    Printable handle(const clang::CXXOperatorCallExpr &operatorCall)
    {

      std::string_view opSpelling{getOperatorSpelling(operatorCall.getOperator())};

      auto args = arguments_of(operatorCall);

      auto *callee = operatorCall.getDirectCallee();
      if (!callee)
      {
        fail_with_message("CXXOperatorCallExpr: No callee");
      }

      std::vector<Printable> template_args;
      auto *templates = callee->getTemplateSpecializationArgs();

      if (templates)
      {

        for (auto template_arg : templates->asArray())
        {
          flat_vector_push_back(template_args, handle(template_arg));
        }
      }

      if (auto *method = llvm::dyn_cast_or_null<clang::CXXMethodDecl>(callee); method != nullptr)
      {

        auto function = handle(*method);

        auto onExpression = args[0];
        args.erase(args.begin());

        return make_printable(
            MemberCall{
                onExpression,
                FunctionCall{
                    function.return_type,
                    function.name,
                    args,
                    template_args,
                    function}});
      }
      else
      {

        auto function = handle(*callee);

        return make_printable(
            FunctionCall{
                function.return_type,
                function.name,
                args,
                template_args,
                function});
      }
    }

    UnaryOperator handle(const clang::UnaryOperator &unary)
    {
      return {
          unary.getOpcodeStr(unary.getOpcode()).str(),
          subexpr_of(unary)};
    }

    Printable handle(const clang::CallExpr &expr)
    {
      auto result =
          match<
              clang::CXXMemberCallExpr,
              clang::CXXOperatorCallExpr>(expr);

      return result ? std::move(result.value()) : make_printable(callExprOnly(expr));
    }

    VariableDeclaration handle(const clang::ParmVarDecl &param)
    {
      return {
          Variable{
              type_of(param),
              qualifiedName(param)}};
    }
  };

} // namespace ct

#endif /* TRANSLATOR_HPP */
