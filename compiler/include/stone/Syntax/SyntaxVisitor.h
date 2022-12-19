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
class Type;

template <typename ImplTy, typename DeclRetTy = void, typename ExprRetTy = void,
          typename StmtRetTy = void, typename TypeRetTy = void,
          typename... Args>
class SyntaxVisitor {
public:
  DeclRetTy Visit(Decl *D, Args... AA) {
    switch (D->GetKind()) {
#define DECL(KIND, PARENT)                                                     \
  case DeclKind::KIND:                                                         \
    return static_cast<ImplTy *>(this)->Visit##KIND##Decl(                     \
        static_cast<KIND##Decl *>(D), ::std::forward<Args>(AA)...);
#include "stone/Syntax/DeclKind.def"
    }
    llvm_unreachable("Not reachable, all cases handled");
  }

#define DECL(KIND, PARENT)                                                     \
  DeclRetTy Visit##KIND##Decl(KIND##Decl *D, Args... AA) {                     \
    return static_cast<ImplTy *>(this)->Visit##PARENT(                         \
        D, ::std::forward<Args>(AA)...);                                       \
  }
#define ABSTRACT_DECL(KIND, PARENT) DECL(KIND, PARENT)
#include "stone/Syntax/DeclKind.def"
};

template <typename ImplTy, typename DeclRetTy = void, typename... Args>
using DeclVisitor = SyntaxVisitor<ImplTy, DeclRetTy, void, void, void, Args...>;

template <typename ImplTy, typename ExprRetTy = void, typename... Args>
using ExprVisitor = SyntaxVisitor<ImplTy, void, ExprRetTy, void, void, Args...>;

template <typename ImplTy, typename StmtRetTy = void, typename... Args>
using StmtVisitor = SyntaxVisitor<ImplTy, void, void, StmtRetTy, void, Args...>;

} // namespace syn
} // namespace stone
#endif
