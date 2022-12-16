#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxVisitor.h"

using namespace stone::sem;

class StmtChecking final : public StmtVisitor<StmtChecking> {
  TypeChecker &checker;

public:
  StmtChecking(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Stmt *s) {}
};

void TypeChecker::CheckStmt(Stmt *s) { StmtChecking(*this).Visit(s); }
