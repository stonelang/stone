#ifndef STONE_AST_ASTVISITOR_H
#define STONE_AST_ASTVISITOR_H

#include "stone/AST/Decl.h"
#include "stone/AST/Expr.h"
#include "stone/AST/Module.h"
#include "stone/AST/QualType.h"
#include "stone/AST/Stmt.h"

namespace stone {

class Decl;
class Stmt;
class Expr;
class Type;

template <typename ImplTy, typename DeclRetTy = void, typename ExprRetTy = void,
          typename StmtRetTy = void, typename TypeRetTy = void,
          typename... Args>
class ASTVisitor {
public:
  DeclRetTy Visit(Decl *D, Args... AA) {
    switch (D->GetKind()) {
#define DECL(KIND, PARENT)                                                     \
  case DeclKind::KIND:                                                         \
    return static_cast<ImplTy *>(this)->Visit##KIND##Decl(                     \
        static_cast<KIND##Decl *>(D), ::std::forward<Args>(AA)...);
#include "stone/AST/DeclKind.def"
    }
    llvm_unreachable("Not reachable, all cases handled");
  }

#define DECL(KIND, PARENT)                                                     \
  DeclRetTy Visit##KIND##Decl(KIND##Decl *D, Args... AA) {                     \
    return static_cast<ImplTy *>(this)->Visit##PARENT(                         \
        D, ::std::forward<Args>(AA)...);                                       \
  }
#define ABSTRACT_DECL(KIND, PARENT) DECL(KIND, PARENT)
#include "stone/AST/DeclKind.def"

public:
  // Force all kinds to be handled at a lower level.
  void VisitDecl(Decl *D) { llvm_unreachable("Unreachable decl"); }
};

template <typename ImplTy, typename DeclRetTy = void, typename... Args>
using DeclVisitor = ASTVisitor<ImplTy, DeclRetTy, void, void, void, Args...>;

template <typename ImplTy, typename ExprRetTy = void, typename... Args>
using ExprVisitor = ASTVisitor<ImplTy, void, ExprRetTy, void, void, Args...>;

template <typename ImplTy, typename StmtRetTy = void, typename... Args>
using StmtVisitor = ASTVisitor<ImplTy, void, void, StmtRetTy, void, Args...>;

} // namespace stone
#endif
