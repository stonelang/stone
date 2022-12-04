#ifndef STONE_SYNTAX_SYNTAXVISITOR_H
#define STONE_SYNTAX_SYNTAXVISITOR_H

namespace stone {
namespace syn {
class Decl;
class Stmt;
class Expr;

template <typename ImplClass, typename ExprRetTy = void,
          typename StmtRetTy = void, typename DeclRetTy = void,
          typename... Args>
class SyntaxVisitor {
public:
  void Visit(Decl *d);
  void Visit(Stmt *s);
  void Visit(Expr *e);
};

template <typename ImplClass, typename ExprRetTy = void, typename... Args>
using ExprVisitor = SyntaxVisitor<ImplClass, ExprRetTy, void, void, Args...>;

template <typename ImplClass, typename StmtRetTy = void, typename... Args>
using StmtVisitor = SyntaxVisitor<ImplClass, void, StmtRetTy, void, Args...>;

template <typename ImplClass, typename DeclRetTy = void, typename... Args>
using DeclVisitor = SyntaxVisitor<ImplClass, void, void, DeclRetTy, Args...>;

} // namespace syn
} // namespace stone
#endif
