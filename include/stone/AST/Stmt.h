#ifndef STONE_AST_STMT_H
#define STONE_AST_STMT_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "stone/AST/ASTNode.h"
#include "stone/AST/IfConfig.h"
#include "stone/AST/StmtKind.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {
class SrcMgr;
class Decl;
class FunDecl;
class VarDecl;
class Expr;
class ASTContext;

class Stmt : public MemoryAllocation<Stmt> {
  StmtKind kind;

public:
  Stmt() = delete;
  Stmt(const Stmt &) = delete;
  Stmt(Stmt &&) = delete;
  Stmt &operator=(const Stmt &) = delete;
  Stmt &operator=(Stmt &&) = delete;

public:
  Stmt(StmtKind kind) : kind(kind) {}

public:
  StmtKind GetKind() const { return kind; }
};

// class DeclStmt : public Stmt {
//   SrcLoc startLoc, endLoc;
// };

/// BraceStmt - A brace enclosed sequence of expressions, stmts, or decls, like
/// { var x = 10; print(10) }.
class BraceStmt final : public Stmt,
                        public llvm::TrailingObjects<BraceStmt, ASTNode> {
  friend TrailingObjects;

  SrcLoc lbLoc;
  SrcLoc rbLoc;

public:
  BraceStmt(SrcLoc lbLoc, llvm::ArrayRef<ASTNode> elements, SrcLoc rbLoc);

public:
  SrcLoc GetLBraceLoc() const { return lbLoc; }
  SrcLoc GetRBraceLoc() const { return rbLoc; }

  // SourceRange getSourceRange() const { return SourceRange(LBLoc, RBLoc); }

  // bool empty() const { return getNumElements() == 0; }
  // unsigned getNumElements() const { return Bits.BraceStmt.NumElements; }

  // // ASTNode getFirstElement() const { return getElements().front(); }
  // ASTNode getLastElement() const { return getElements().back(); }

  // void setFirstElement(ASTNode node) { getElements().front() = node; }
  // void setLastElement(ASTNode node) { getElements().back() = node; }

  /// The elements contained within the BraceStmt.
  // llvm::MutableArrayRef<ASTNode> getElements() {
  //   return {getTrailingObjects<ASTNode>(), Bits.BraceStmt.NumElements};
  // }

  /// The elements contained within the BraceStmt (const version).
  // llvm::ArrayRef<ASTNode> getElements() const {
  //   return {getTrailingObjects<ASTNode>(), Bits.BraceStmt.NumElements};
  // }

public:
  static bool classof(const Stmt *stmt) {
    return stmt->GetKind() == StmtKind::Brace;
  }

public:
  static BraceStmt *Create(SrcLoc lbloc, llvm::ArrayRef<ASTNode> elements,
                           SrcLoc rbloc, ASTContext &astContext,
                           std::optional<bool> implicit = false);
};

class SwitchCaseStmt : public Stmt {
protected:
  /// The location of the ":".
  SrcLoc colonLoc;
};

/// CaseStmt - Represent a case statement. It can optionally be a GNU case
/// statement of the form LHS ... RHS representing a range of cases.
class CaseStmt final : public SwitchCaseStmt,
                       private llvm::TrailingObjects<CaseStmt, Stmt *, SrcLoc> {
  friend TrailingObjects;
};

class DefaultStmt : public SwitchCaseStmt {
  Stmt *subStmt;
};

class NextStmt : public Stmt {
  Stmt *subStmt;
};

class ValueStmt : public Stmt {
public:
  ValueStmt(StmtKind kind) : Stmt(kind) {}
};
/// LabelStmt - Represents a label, which has a substatement.  For example:
///    foo: return;
class LabelStmt : public ValueStmt {
  // LabelDecl *labelDecl;
  Stmt *subStmt;
};

/// IfStmt - This represents an if/then/else.
class IfStmt final : public Stmt,
                     private llvm::TrailingObjects<IfStmt, Stmt *, SrcLoc> {
  friend TrailingObjects;

public:
};

class NullStmt final : public Stmt,
                       private llvm::TrailingObjects<NullStmt, Stmt *, SrcLoc> {
  friend TrailingObjects;

public:
};

class DeferStmt : public Stmt {
  ///
  SrcLoc deferLoc;
  /// This is the bound temp function.
  FunDecl *tempDecl;

  /// This is the invocation of the closure, which is to be emitted on any
  /// error paths.
  Expr *callExpr;

public:
};

class ReturnStmt : public Stmt {
  Expr *result;
  SrcLoc returnLoc;

public:
  ReturnStmt(SrcLoc returnLoc, Expr *result)
      : Stmt(StmtKind::Return), returnLoc(returnLoc), result(result) {}

  SrcLoc GetReturnLoc() const { return returnLoc; }
  SrcLoc GetStartLoc() const;
  SrcLoc GetEndLoc() const;

  bool HasResult() const { return result != 0; }

  Expr *GetResult() const {
    assert(result && "ReturnStmt doesn't have a result");
    return result;
  }
  void SetResult(Expr *e) { result = e; }

  static bool classof(const Stmt *stmt) {
    return stmt->GetKind() == StmtKind::Return;
  }
};

} // namespace stone
#endif
