#include "stone/Sem/TypeChecker.h"
#include "stone/AST/Expr.h"
#include "stone/AST/ASTVisitor.h"

using namespace stone::sem;
using namespace stone::ast;

class ExprChecking final : public ExprVisitor<ExprChecking> {
  TypeChecker &checker;

public:
  ExprChecking(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Expr *e) {}
};

void TypeChecker::CheckExpr(Expr *e) { ExprChecking(*this).Visit(e); }
