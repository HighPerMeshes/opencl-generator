#ifndef HANDLEDISPATCHEREXECUTES_HPP
#define HANDLEDISPATCHEREXECUTES_HPP

#include <clang-frontend.hpp>
#include <clang-frontend/visitors/HandleExecuteArgument.hpp>

#include <clang/Tooling/Tooling.h>


namespace ct
{

struct HandleDispatcherExecutes : public Visitor<HandleDispatcherExecutes>
{

  std::fstream& output;
  std::vector<std::string> kernel_names;

  bool VisitCXXMemberCallExpr(clang::CXXMemberCallExpr *expr)
  {
    using namespace clang;
    
    auto *callee = expr->getDirectCallee();

    if (callee == nullptr || callee->getNameAsString() != "Execute")
      return true;

    std::stringstream replacement_text;
    
    

    replacement_text << "{\n";
    
    std::vector<std::string> hpm_ocl_kernel_names;

    auto type = expr->getArg(0)->getType();
    auto full_name = type.getAsString(environment.context.getPrintingPolicy());
    auto name = remove_templates(full_name);

    std::string range_type {  
      std::next(std::find(full_name.begin(), full_name.end(), '<')),
      std::find(full_name.begin(), full_name.end(), ',')
    };

    size_t kernel_number = 0;
    for_args(expr, [&, this](const Expr *e) {
      auto type = e->getType();
      auto name = remove_templates(
          type.getAsString(environment.context.getPrintingPolicy()));

      if (name != "HPM::MeshLoop")
        return;

      HandleExecuteArgument handler {environment, output }; 
      handler.TraverseCXXRecordDecl(
          type->getAsCXXRecordDecl());

      auto kernel_text = environment.get_text(e->getSourceRange());      

      std::string hpm_kernel_name = std::string { "hpm_kernel_" } + std::to_string(kernel_number);
      std::string buffers_name = std::string { "buffers_" } + std::to_string(kernel_number);
      std::string hpm_ocl_kernel_name = std::string { "hpm_ocl_kernel_" } + std::to_string(kernel_number);
      std::string offset_name = std::string { "offsets_" } + std::to_string(kernel_number);
      hpm_ocl_kernel_names.emplace_back(hpm_ocl_kernel_name);

      replacement_text 
        << "\tauto " << hpm_kernel_name << " = " << kernel_text << ";\n"
        << "\tauto " << buffers_name << " = GetBuffers(" << hpm_kernel_name << ");\n"
        << "\tauto " << offset_name << " = GetOffsets(" << hpm_kernel_name << ");\n"
        << "\tauto " << hpm_ocl_kernel_name << " = HPM::OpenCLKernelEnqueuer { hpm_ocl, \"" << handler.kernel_name << "\", std::tuple<" << range_type << "> { 0 }, " << hpm_kernel_name << ".entity_range.GetSize() }.with(" << buffers_name << ").with(" << offset_name << ")";

      kernel_names.emplace_back(handler.kernel_name);

      for(const auto& member : handler.member_names) {
        replacement_text << ".with(std::tuple { " << member << "(" << hpm_kernel_name << ".entity_range, hpm_ocl)})";
      }
      replacement_text << "\n;";

      kernel_number++;
    });

    replacement_text << "\tHPM::OpenCLDispatcher {}.Dispatch(";
    
    
    if(unqualify(Name { name }) == Name { "Range" }) {
      replacement_text << environment.get_text(expr->getArg(0)->getSourceRange()) <<  ", ";
    }
    else {
      replacement_text << "HPM::iterator::Range { 0 }, ";
    }

    print_with_delimiter(replacement_text, hpm_ocl_kernel_names, ", ");

    replacement_text << ");\n";

    replacement_text << "}\n";

    environment.modifications.replace_range(
      expr->getSourceRange(),
      replacement_text.str()
    );

    return true;
  }
};

} // namespace ct

#endif /* HANDLEDISPATCHEREXECUTES_HPP */
