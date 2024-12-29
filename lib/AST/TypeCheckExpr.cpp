#include "stone/AST/ASTVisitor.h"
#include "stone/AST/Expr.h"
#include "stone/AST/TypeChecker.h"

using namespace stone;

// class ExprChecking final : public ExprVisitor<ExprChecking> {
//   TypeChecker &checker;

// public:
//   ExprChecking(TypeChecker &checker) : checker(checker) {}

// public:
//   void Visit(Expr *e) {}
// };

// void TypeChecker::CheckExpr(Expr *e) { ExprChecking(*this).Visit(e); }