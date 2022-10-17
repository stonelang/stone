#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/SyntaxVisitor.h"

using namespace stone::sem;
using namespace stone::syn;

class ExprChecker final : public ExprVisitor<ExprChecker> {
  TypeChecker &checker;

public:
  ExprChecker(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Expr *e) {}
};

void TypeChecker::CheckExpr(Expr *e) { ExprChecker(*this).Visit(e); }
