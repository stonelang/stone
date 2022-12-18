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

template <typename ImplClass, typename DeclRetTy = void,
          typename ExprRetTy = void, typename StmtRetTy = void,
          typename TypeRetTy = void, typename... Args>
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

public:
  TypeRetTy Visit(TypeBase *T, Args... AA) {
    switch (T->GetKind()) {
#define TYPE(CLASS, PARENT)                                                    \
  case TypeKind::CLASS:                                                        \
    return static_cast<ImplClass *>(this)->Visit##CLASS##Type(                 \
        static_cast<CLASS##Type *>(T), ::std::forward<Args>(AA)...);
#include "stone/Syntax/TypeKind.def"
    }
    llvm_unreachable("Not reachable, all cases handled");
  }

#define TYPE(CLASS, PARENT)                                                    \
  TypeRetTy Visit##CLASS##Type(CLASS##Type *T, Args... AA) {                   \
    return static_cast<ImplClass *>(this)->Visit##PARENT(                      \
        T, ::std::forward<Args>(AA)...);                                       \
  }
#define ABSTRACT_TYPE(CLASS, PARENT) TYPE(CLASS, PARENT)
#include "stone/Syntax/TypeKind.def"
};

template <typename ImplClass, typename DeclRetTy = void, typename... Args>
using DeclVisitor =
    SyntaxVisitor<ImplClass, DeclRetTy, void, void, void, Args...>;

template <typename ImplClass, typename ExprRetTy = void, typename... Args>
using ExprVisitor =
    SyntaxVisitor<ImplClass, void, ExprRetTy, void, void, Args...>;

template <typename ImplClass, typename StmtRetTy = void, typename... Args>
using StmtVisitor =
    SyntaxVisitor<ImplClass, void, void, StmtRetTy, void, Args...>;

// template <typename ImplClass, typename TypeRetTy = void, typename... Args>
// using TypeVisitor =
//     SyntaxVisitor<ImplClass, void, void, void, TypeRetTy, Args...>;

} // namespace syn
} // namespace stone
#endif
