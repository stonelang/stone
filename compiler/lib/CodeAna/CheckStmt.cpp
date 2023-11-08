#include "stone/AST/ASTVisitor.h"
#include "stone/CodeAna/TypeChecker.h"

class StmtChecking final : public StmtVisitor<StmtChecking> {
  TypeChecker &checker;

public:
  StmtChecking(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Stmt *s) {}

  // void VisitIfStmt() {
  // }
};

void TypeChecker::CheckStmt(Stmt *s) { StmtChecking(*this).Visit(s); }
