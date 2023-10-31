#ifndef STONE_ASTEXPR_H
#define STONE_ASTEXPR_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "stone/AST/ASTWalker.h"
#include "stone/AST/Stmt.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {
namespace ast {
class ASTWalker;

class Expr : public ValueStmt {
public:
  Expr() = delete;
  Expr(const Expr &) = delete;
  Expr(Expr &&) = delete;
  Expr &operator=(const Expr &) = delete;
  Expr &operator=(Expr &&) = delete;

public:
  Expr(StmtKind kind) : ValueStmt(kind) {}

  // StmtKind kind, QualType qualTy, ExprValueKind VK, ExprObjectKind OK

  /// This recursively walks the AST rooted at this expression.
  Expr *Walk(ASTWalker &walker);
  Expr *Walk(ASTWalker &&walker) { return Walk(walker); }
};

/// FullExpr - Represents a "full-expression" node.
// class FullExpr : public Expr {
// protected:
//   Stmt *subExpr;
// };

/// Represents a new-expression for memory allocation and constructor
/// calls, e.g: "new NewExpr(foo)".
class NewExpr final : public Expr,
                      private llvm::TrailingObjects<NewExpr, Stmt *, SrcRange> {
  friend TrailingObjects;

public:
  NewExpr() : Expr(StmtKind::New) {}

public:
  static NewExpr *Create();
};

// class ThisExpr : public Expr {
// public:
// };

/// Represents a call to a constructor.
// class ConstructorExpr : public Expr {
// public:
// };
// /// Represents a call to a destructor.
// class DestructorExpr : public Expr {
// public:
// };

// class ParenExpr final : public Expr {
//   Stmt *val;
//   SrcLoc leftLoc, rightLoc;

// public:
//   ParenExpr(SrcLoc left, SrcLoc right, Expr *val)
//       : Expr(ParenExprClass, val->getType(), val->getValueKind(),
//              val->getObjectKind()),
//         L(l), R(r), Val(val) {
//     setDependence(computeDependence(this));
//   }

//   /// Construct an empty parenthesized expression.
//   explicit ParenExpr(EmptyShell Empty)
//     : Expr(ParenExprClass, Empty) { }

//   const Expr *GetSubExpr() const { return cast<Expr>(al); }
//   Expr *GetSubExpr() { return cast<Expr>(Val); }
//   void SetSubExpr(Expr *E) { Val = E; }

//   SourceLocation GetBeginLoc() const LLVM_READONLY { return beginLoc; }
//   SourceLocation GetEndLoc() const LLVM_READONLY { return endLoc; }

//   /// Get the location of the left parentheses '('.
//   SourceLocation GetLeftParen() const { return L; }
//   void SetLParen(SourceLocation Loc) { L = Loc; }

//   /// Get the location of the right parentheses ')'.
//   SourceLocation GetRightParen() const { return R; }
//   void setRParen(SourceLocation Loc) { R = Loc; }

// static bool classof(const Stmt *T) {
//   return T->getStmtClass() == ParenExprClass;
// }

// // Iterators
// child_range children() { return child_range(&Val, &Val+1); }
// const_child_range children() const {
//   return const_child_range(&Val, &Val + 1);
// }
//};

/// LiteralExpr - Common base class between the literals.
// class LiteralExpr : public Expr {};

// class NullLiteralExpr : public LiteralExpr {
// public:
// };

class IdentityExpr : public Expr {
  Expr *subExpr;
};

// DotThis
class DotThisExpr : public IdentityExpr {};

} // namespace ast
} // namespace stone
#endif
