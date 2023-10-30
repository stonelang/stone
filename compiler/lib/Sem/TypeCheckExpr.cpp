#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/SyntaxVisitor.h"

using namespace stone::sem;
using namespace stone::syn;

class ExprChecking final : public ExprVisitor<ExprChecking> {
  TypeChecker &checker;

public:
  ExprChecking(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Expr *e) {}
};

void TypeChecker::CheckExpr(Expr *e) { ExprChecking(*this).Visit(e); }
