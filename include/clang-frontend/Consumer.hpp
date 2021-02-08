#ifndef CONSUMER_HPP
#define CONSUMER_HPP

#include <memory>

#include "clang/Tooling/Tooling.h"


template <typename Op> struct Consumer : public clang::ASTConsumer {
public:
  Consumer(Op op) : op{op} {}

  virtual void HandleTranslationUnit(clang::ASTContext &context) override {
    op(context);
  }

  std::unique_ptr<clang::ASTConsumer> newASTConsumer() {
    return std::make_unique<Consumer>(op);
  }

  const Op op;
};

#endif // !CONSUMER_HPP