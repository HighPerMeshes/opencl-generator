#ifndef MODIFICATIONS_HPP
#define MODIFICATIONS_HPP

#include "clang/Tooling/Tooling.h"

#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Core/Replacement.h"

class Modifications {

  public:

  bool ignore_outputs { true };

  Modifications(clang::SourceManager& sm, clang::LangOptions opts) : sm { sm }, opts { opts } {}

  void replace_range(clang::SourceRange range, llvm::StringRef replacement_text) {
    add_modification(replacement(range, replacement_text));
  }

  void try_replace_range(clang::SourceRange range, llvm::StringRef replacement_text) {
    try_replacements.push_back(replacement(range, replacement_text));
  }

  void insert_text(clang::SourceLocation location, llvm::StringRef insert_text) {
    add_modification(insertion(location, insert_text));
  }

  void try_insert_text(clang::SourceLocation location, llvm::StringRef insert_text) {
    try_replacements.push_back(insertion(location, insert_text));
  }

  auto calculate_rewriter() {

    clang::Rewriter rewriter;
    rewriter.setSourceMgr(sm, opts);
    
    for(const auto& replacement : try_replacements) {
      auto failure = replacements.add(replacement);

      if(!ignore_outputs && failure) {
        llvm::errs() << failure << "\n";
      }

    }

    for(const auto& replacement : replacements) {
      replacement.apply(rewriter);
    }

    return rewriter;

  }

  private: 

  void add_modification(const clang::tooling::Replacement& replacement) {
    auto failure = replacements.add( replacement );
      if (failure) {
        llvm::errs() << failure;
        std::exit(EXIT_FAILURE);
    }
  }

  clang::tooling::Replacement replacement(clang::SourceRange range, llvm::StringRef replacement_text) {
    return { sm, clang::CharSourceRange { range, true }, replacement_text };
  }

  clang::tooling::Replacement insertion(clang::SourceLocation location, llvm::StringRef insert_text) {
    return { sm, location, 0, insert_text };
  }

  clang::SourceManager& sm;
  clang::LangOptions opts;
  clang::tooling::Replacements replacements;
  std::vector<clang::tooling::Replacement> try_replacements;

};

#endif // !MODIFICATIONS_HPP
