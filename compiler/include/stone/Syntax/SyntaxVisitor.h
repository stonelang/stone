#ifndef STONE_SYNTAX_SYNTAXVISITOR_H
#define STONE_SYNTAX_SYNTAXVISITOR_H

#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/Type.h"

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
  DeclRetTy Visit(Decl *D, Args... AA) {
    switch (D->GetKind()) {
#define DECL(CLASS, PARENT)                                                    \
  case DeclKind::CLASS:                                                        \
    return static_cast<ImplClass *>(this)->Visit##CLASS##Decl(                 \
        static_cast<CLASS##Decl *>(D), ::std::forward<Args>(AA)...);
#include "stone/Syntax/DeclKind.def"
    }
    llvm_unreachable("Not reachable, all cases handled");
  }

#define DECL(CLASS, PARENT)                                                    \
  DeclRetTy Visit##CLASS##Decl(CLASS##Decl *D, Args... AA) {                   \
    return static_cast<ImplClass *>(this)->Visit##PARENT(                      \
        D, ::std::forward<Args>(AA)...);                                       \
  }
#define BASE_DECL(CLASS, PARENT) DECL(CLASS, PARENT)
#include "stone/Syntax/DeclKind.def"
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
