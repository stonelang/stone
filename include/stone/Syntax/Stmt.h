#ifndef STONE_SYNTAX_STMT_H
#define STONE_SYNTAX_STMT_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "stone/Core/LLVM.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Syntax/StmtBits.h"
#include "stone/Syntax/StmtKind.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {
class SrcMgr;

namespace syn {
class Decl;
class FunDecl;
class VarDecl;
class Expr;
class StringLiteral;
class SyntaxContext;

class Stmt {
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
  StmtKind GetKind() { return kind; }
};

class DeclStmt : public Stmt {
  SrcLoc startLoc, endLoc;
};

/// This is equivalent to the CompoundStmt (c/c++) that
/// represents a group of statements like { stmt stmt }.
class BlockStmt final : public Stmt,
                        private llvm::TrailingObjects<BlockStmt, Stmt *> {
public:
};

class SwitchCase : public Stmt {
protected:
  /// The location of the ":".
  SrcLoc colonLoc;
};

/// CaseStmt - Represent a case statement. It can optionally be a GNU case
/// statement of the form LHS ... RHS representing a range of cases.
class CaseStmt final : public SwitchCase,
                       private llvm::TrailingObjects<CaseStmt, Stmt *, SrcLoc> {
  friend TrailingObjects;
};

class DefaultStmt : public SwitchCase {
  Stmt *subStmt;
};

class NextStmt : public Stmt {
  Stmt *subStmt;
};

class ValueStmt : public Stmt {
public:
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

  /// This is the invocation of the closure, which is to be emitted on any error
  /// paths.
  Expr *callExpr;

public:
};

} // namespace syn
} // namespace stone
#endif
