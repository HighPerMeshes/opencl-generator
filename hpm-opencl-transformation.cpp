#include <clang/Tooling/CommonOptionsParser.h>

#include "util.hpp"

#include <ir.hpp>
#include <traversal.hpp>
#include <clang-frontend.hpp>
#include <clang-frontend/visitors.hpp>

#include <numeric>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <util/flat_vector.hpp>
#include <fstream>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace ct;

struct AddAllocator : public Visitor<AddAllocator>
{

  bool VisitVarDecl(clang::VarDecl *decl)
  {
    using namespace clang;

    auto type = decl->getType();
    auto type_name = remove_templates(
        type.getAsString(environment.context.getPrintingPolicy()));

    if (type_name == "HPM::drts::Runtime" || type_name == "drts::Runtime" || type_name == "Runtime")
    {
      auto var_name = decl->getName();
      std::stringstream replacement;
      replacement << "HPM::drts::Runtime " << var_name.str() << "{ HPM::GetBuffer<HPM::OpenCLHandler::SVMAllocator>{} }";
      environment.modifications.replace_range(decl->getSourceRange(), replacement.str());
    }

    return true;
  }

  bool VisitCXXMemberCallExpr(clang::CXXMemberCallExpr *expr)
  {

    auto *callee = expr->getDirectCallee();

    if (callee == nullptr || callee->getNameAsString() != "GetBuffer")
      return true;

    auto *templates = callee->getTemplateSpecializationArgs();
    if (!templates)
      return true;
    auto first_arg = templates->asArray()[0];
    auto name = first_arg.getAsType().getAsString(environment.context.getPrintingPolicy());

    std::stringstream replacement;
    replacement << ", hpm_ocl.GetSVMAllocator<" << name << ">()";

    environment.modifications.try_insert_text(
        expr->getEndLoc(),
        replacement.str());

    return true;
  }
};

struct AddBoilerplate : public Visitor<AddBoilerplate>
{

  std::vector<std::string> kernel_names;

  bool VisitFunctionDecl(clang::FunctionDecl *decl)
  {

    if (decl->getNameAsString() != "main")
    {
      return true;
    }

    std::stringstream replacement;
    replacement << "const HPM::auxiliary::ConfigParser hpm_config_parser(\"config.cfg\");\n"
                << "const std::string hpm_ocl_platform_name = hpm_config_parser.GetValue<std::string>(\"oclPlatformName\");\n"
                << "const std::string hpm_ocl_device_name = hpm_config_parser.GetValue<std::string>(\"oclDeviceName\");\n"
                << "std::fstream hpm_kernel_stream { \"output.cl\" };\n"
                << "std::string hpm_kernel_string((std::istreambuf_iterator<char>(hpm_kernel_stream)), std::istreambuf_iterator<char>());\n"
                << "HPM::OpenCLHandler hpm_ocl(hpm_ocl_platform_name, hpm_ocl_device_name);\n"
                << "hpm_ocl.LoadKernelsFromString(hpm_kernel_string, {";

    std::vector<std::string> transformed_names(kernel_names.size());
    std::transform(kernel_names.begin(), kernel_names.end(), transformed_names.begin(), [](const auto &name) {
      return std::string{"\""} + name + "\"";
    });

    print_with_delimiter(replacement, transformed_names, ", ");

    replacement << "});\n";

    environment.modifications.insert_text(
        decl->getBody()->getBeginLoc().getLocWithOffset(1),
        replacement.str());

    return true;
  }
};

int main(int argc, const char **argv)
{
  cl::OptionCategory toolCategory("hpm-opencl-transformation options");
  CommonOptionsParser OptionsParser(argc, argv, toolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  std::fstream output("output.cl", std::fstream::out);
  std::fstream host_output("host.cpp", std::fstream::out);

  auto action = Consumer{[&](clang::ASTContext &context) {
//     const char *Vector3 = "struct Vector3 { \
//     double values[3]; \
// }; \
// struct Vector3 make_Vector3(double arg_1, double arg_2, double arg_3) { \
//     struct Vector3 vec = { .values = { arg_1, arg_2, arg_3 } }; \
//     return vec; \
// } \
// void add_assign_Vector3(struct Vector3 * lhs, struct Vector3 rhs) { \
//     for(int i = 0; i < 3; ++i) { \
//         lhs->values[i] += rhs.values[i]; \
//     } \
// } \
// void add_scalar_assign_Vector3(struct Vector3 * lhs, double rhs) { \
//     for(int i = 0; i < 3; ++i) { \
//         lhs->values[i] += rhs; \
//     } \
// } \
// void times_assign_Vector3(struct Vector3 * lhs, struct Vector3 rhs) { \
//     for(int i = 0; i < 3; ++i) { \
//         lhs->values[i] *= rhs.values[i]; \
//     } \
// } \
// void times_scalar_assign_Vector3(struct Vector3 * lhs, double rhs) { \
//     for(int i = 0; i < 3; ++i) { \
//         lhs->values[i] *= rhs; \
//     } \
// } \
// void divides_assign_Vector3(struct Vector3 * lhs, struct Vector3 rhs) { \
//     for(int i = 0; i < 3; ++i) { \
//         lhs->values[i] /= rhs.values[i]; \
//     } \
// } \
// void minus_assign_Vector3(struct Vector3 * lhs, struct Vector3 rhs) { \
//     for(int i = 0; i < 3; ++i) { \
//         lhs->values[i] -= rhs.values[i]; \
//     } \
// } \
// struct Vector3 add_Vector3(struct Vector3 lhs, struct Vector3 rhs) { \
//     add_assign_Vector3(&lhs, rhs); \
//     return lhs; \
// } \
// struct Vector3 times_Vector3(struct Vector3 lhs, struct Vector3 rhs) { \
//     times_assign_Vector3(&lhs, rhs); \
//     return lhs; \
// } \
// struct Vector3 add_scalar_Vector3(struct Vector3 lhs, double rhs) { \
//     add_scalar_assign_Vector3(&lhs, rhs); \
//     return lhs; \
// } \
// struct Vector3 times_scalar_Vector3(struct Vector3 lhs, double rhs) { \
//     times_scalar_assign_Vector3(&lhs, rhs); \
//     return lhs; \
// } \
// struct Vector3 divides_Vector3(struct Vector3 lhs, struct Vector3 rhs) { \
//     divides_assign_Vector3(&lhs, rhs); \
//     return lhs; \
// } \
// struct Vector3 minus_Vector3(struct Vector3 lhs, struct Vector3 rhs) { \
//     minus_assign_Vector3(&lhs, rhs); \
//     return lhs; \
// }";

  // output << Vector3;

    Environment e{context};
    auto dispatcher_handler = HandleDispatcherExecutes{e, output};
    dispatcher_handler.TraverseDecl(context.getTranslationUnitDecl());
    AddAllocator{e}.TraverseDecl(context.getTranslationUnitDecl());
    AddBoilerplate{e, dispatcher_handler.kernel_names}.TraverseDecl(context.getTranslationUnitDecl());
    auto rewriter = e.modifications.calculate_rewriter();

    const auto *buffer =
        rewriter.getRewriteBufferFor(e.sourceManager.getMainFileID());
    host_output << std::string{buffer->begin(), buffer->end()} << std::endl;
  }};
  Tool.run(newFrontendActionFactory(&action).get());
}
