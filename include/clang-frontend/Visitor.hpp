#ifndef VISITOR_HPP
#define VISITOR_HPP

#include "Environment.hpp"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Tooling/Tooling.h"
#include "functional"

template <typename Derived>
struct Visitor : public clang::RecursiveASTVisitor<Derived>
{

  Environment &environment;

  Visitor(Environment &environment) : environment{environment} {}
};

#endif // !VISITOR_HPP