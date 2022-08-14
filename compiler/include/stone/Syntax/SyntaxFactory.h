#ifndef STONE_SYNTAX_SYNTAXFACTORY_H
#define STONE_SYNTAX_SYNTAXFACTORY_H

#include "stone/Basic/SrcLoc.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxResult.h"
#include "stone/Syntax/Type.h"

namespace stone {

namespace syn {
class Decl;
class DeclContext;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class SwitchStmt;
class Expr;
class SyntaxFile;

class SyntaxFactory final {
public:
  SyntaxFactory(const SyntaxFactory &) = delete;
  SyntaxFactory(SyntaxFactory &&) = delete;
  SyntaxFactory &operator=(const SyntaxFactory &) = delete;
  SyntaxFactory &operator=(SyntaxFactory &&) = delete;
  // SyntaxFactory() = delete;

public:
  SyntaxFactory();
  ~SyntaxFactory();
};

class DeclSyntaxFactory final {
public:
  DeclSyntaxFactory(const DeclSyntaxFactory &) = delete;
  DeclSyntaxFactory(DeclSyntaxFactory &&) = delete;
  DeclSyntaxFactory &operator=(const DeclSyntaxFactory &) = delete;
  DeclSyntaxFactory &operator=(DeclSyntaxFactory &&) = delete;

public:
};

class StmtSyntaxFactory final {
public:
  StmtSyntaxFactory(const StmtSyntaxFactory &) = delete;
  StmtSyntaxFactory(StmtSyntaxFactory &&) = delete;
  StmtSyntaxFactory &operator=(const StmtSyntaxFactory &) = delete;
  StmtSyntaxFactory &operator=(StmtSyntaxFactory &&) = delete;

public:
};

class ExprSyntaxFactory final {
public:
  ExprSyntaxFactory(const ExprSyntaxFactory &) = delete;
  ExprSyntaxFactory(ExprSyntaxFactory &&) = delete;
  ExprSyntaxFactory &operator=(const ExprSyntaxFactory &) = delete;
  ExprSyntaxFactory &operator=(ExprSyntaxFactory &&) = delete;

public:
};

} // namespace syn
} // namespace stone
#endif
