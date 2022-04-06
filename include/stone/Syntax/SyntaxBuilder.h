#ifndef STONE_SYNTAX_SYNTAXBUILDER_H
#define STONE_SYNTAX_SYNTAXBUILDER_H

#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/Specifier.h"

namespace stone {
namespace syn {

class Decl;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class MatchStmt;
class Expr;
class BraceStmt;
class Syntax;

class SyntaxBuilder {
  Syntax &syntax;

public:
  SyntaxBuilder(Syntax &syntax) : syntax(syntax) {}

public:
  Syntax &GetSyntax() { return syntax; }
};

class StmtSyntaxBuilder : public SyntaxBuilder {
public:
  StmtSyntaxBuilder(Syntax &syntax);
};

class BlockStmtSyntaxBuilder : public StmtSyntaxBuilder {
public:
  BlockStmtSyntaxBuilder(Syntax &syntax);

public:
  void WithLeftBrace();
  void WithRightBrace();
};

class DeclSyntaxBuilder : public SyntaxBuilder {
public:
  DeclSyntaxBuilder(Syntax &syntax);

public:
  void WithIdentifier();
};
class FunDeclSyntaxBuilder final : public DeclSyntaxBuilder,
                                   public BlockStmtSyntaxBuilder {
  SrcLoc funLoc;
  AccessLevel accessLevel; 

public:
  FunDeclSyntaxBuilder(const FunDeclSyntaxBuilder &) = delete;
  FunDeclSyntaxBuilder(FunDeclSyntaxBuilder &&) = delete;
  FunDeclSyntaxBuilder &operator=(const FunDeclSyntaxBuilder &) = delete;
  FunDeclSyntaxBuilder &operator=(FunDeclSyntaxBuilder &&) = delete;

public:
  FunDeclSyntaxBuilder(Syntax &syntax);

public:
  void WithTemplate();
  void WithFunKeyword(SrcLoc loc);
  void WithParams();
  void WithReturnType();
  void WithReturnStmt();
  void WithAccessLevel(AccessLevel level);

public:
  FunDecl *Build();
};

class StructDeclSyntaxBuilder final : public DeclSyntaxBuilder,
                                      public BlockStmtSyntaxBuilder {
public:
  StructDeclSyntaxBuilder(const StructDeclSyntaxBuilder &) = delete;
  StructDeclSyntaxBuilder(StructDeclSyntaxBuilder &&) = delete;
  StructDeclSyntaxBuilder &operator=(const StructDeclSyntaxBuilder &) = delete;
  StructDeclSyntaxBuilder &operator=(StructDeclSyntaxBuilder &&) = delete;

public:
  StructDeclSyntaxBuilder(Syntax &syntax);
  StructDecl *Build();

public:
  void WithStructKeyword();
};

// class ModuleDeclSyntaxBuilder final : public SyntaxBuilder {
// public:
// };

class ExprSyntaxBuilder : public SyntaxBuilder {
public:
  ExprSyntaxBuilder(Syntax &syntax);
};

} // namespace syn
} // namespace stone

#endif
