#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "clang/Tooling/Tooling.h"

#include "Modifications.hpp"

#include <map>
#include <string>

struct Environment {

  Environment(clang::ASTContext &context)
      : context{context}, sourceManager{context.getSourceManager()},
        modifications{sourceManager, context.getLangOpts() } {}

  clang::ASTContext &context;
  clang::SourceManager &sourceManager;
  Modifications modifications;
  
  auto get_text(const clang::SourceRange &range) -> std::string {
    return clang::Lexer::getSourceText({range, true}, sourceManager,
                                context.getLangOpts()).str();
  }

};


#endif // !ENVIRONMENT_HPP