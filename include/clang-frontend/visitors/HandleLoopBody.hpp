#ifndef HANDLELOOPBODY_HPP
#define HANDLELOOPBODY_HPP

#include <clang/Tooling/Tooling.h>
#include <ir.hpp>

namespace ct
{

struct HandleLoopBody : public Visitor<HandleLoopBody>
{
  
  Translator translator { environment };
  
  bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl *decl)
  {
    using namespace clang;

    if(decl->specializations().end() - decl->specializations().begin() != 1) {
      fail_with_message("HandleLoopBody: decl has more than one specialization.");
    }

    auto *method = dyn_cast<CXXMethodDecl>(*decl->specializations().begin());

    if (!method)
      return true;

    kernel = translator.handle(*method);

    return true;
  }

  Function kernel = empty_function();

};

} // namespace ct

#endif /* HANDLELOOPBODY_HPP */
