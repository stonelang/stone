#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxVisitor.h"

using namespace stone::sem;

class StmtChecker final : public StmtVisitor<StmtChecker> {
  TypeChecker &checker;

public:
  StmtChecker(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Stmt *s) {}
};

void TypeChecker::CheckStmt(Stmt *s) { StmtChecker(*this).Visit(s); }
