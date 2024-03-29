#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/ASTVisitor.h"
#include "stone/Syntax/Expr.h"

using namespace stone;

class ExprChecking final : public ExprVisitor<ExprChecking> {
  TypeChecker &checker;

public:
  ExprChecking(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Expr *e) {}
};

void TypeChecker::CheckExpr(Expr *e) { ExprChecking(*this).Visit(e); }
