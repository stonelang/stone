#ifndef STONE_SYNTAX_SYNTAXVISITOR_H
#define STONE_SYNTAX_SYNTAXVISITOR_H

namespace stone {
namespace syn {
class Decl;
class Stmt;
class Expr;

class SyntaxVisitor;

class VisitableSyntax {
public:
  virtual void Visit(SyntaxVisitor &visitor) = 0;
};

class SyntaxVisitor {
public:
  void Visit(const Decl *d);
  void Visit(const Stmt *s);
  void Visit(const Expr *e);
};
} // namespace syn
} // namespace stone
#endif
