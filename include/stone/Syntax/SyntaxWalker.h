#ifndef STONE_SYNTAX_SYNTAXWALKER_H
#define STONE_SYNTAX_SYNTAXWALKER_H

namespace stone {
namespace syn {

class Decl;
class Stmt;
class Expr; 

class SyntaxWalker;
class WalkableSyntax {
public:
  virtual bool Walk(SyntaxWalker &waker) = 0;
};
class SyntaxWalker {
public:
  void Walk(const Decl* d);
  void Walk(const Stmt* s);
  void Walk(const Expr* e);
};
} // namespace syn
} // namespace stone
#endif
