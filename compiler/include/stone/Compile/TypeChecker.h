#ifndef STONE_SEM_TYPECHECKER_H
#define STONE_SEM_TYPECHECKER_H

#include "stone/AST/ASTContext.h"
#include "stone/AST/Module.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Lang.h"

#include <memory>

using namespace stone::ast;

namespace stone {
class TypeCheckerOptions;
class TypeCheckerListener;

namespace ast {
class ASTFile;
class Expr;
class Decl;
class Stmt;
} // namespace ast

namespace typecheck {

class TypeChecker;
class TypeCheckerStats final : public Stats {
  const TypeChecker &checker;

public:
  TypeCheckerStats(const TypeChecker &checker)
      : Stats("checker statistics:"), checker(checker) {}
  void Print(ColorStream &stream) override;
};

class TypeCheckerDiagnostics final {
public:
};

class TypeChecker final {
  friend TypeCheckerStats;

  ASTContext &sc;
  TypeCheckerOptions &typeCheckerOpts;
  std::unique_ptr<TypeCheckerStats> stats;
  TypeCheckerListener *pipeline;

public:
  TypeChecker(const TypeChecker &) = delete;
  TypeChecker &operator=(const TypeChecker &) = delete;
  ~TypeChecker();

public:
  /// TODO: Pass in AST so that you can create the Nodes
  TypeChecker(ASTContext &sc, TypeCheckerOptions &typeCheckerOpts,
              TypeCheckerListener *pipeline = nullptr);

public:
  void CheckASTNode(ASTNode &asttaxNode, DeclContext *dc,
                    bool checkBody = false);

public:
  void CheckDecl(Decl *d);
  void CheckAccessLevel(Decl *D);

public:
  void CheckStmt(Stmt *s);

public:
  void CheckExpr(Expr *e);

public:
  void CheckTypes(Decl *d);
  void CheckTypes(Stmt *stmt, DeclContext *DC);
  void CheckTypes(AliasDecl *alias);
  void CheckTypes(TrailingWhereClause *whereClause);
  void CheckTypes(GenericParamList *params);

public:
  void ComputeAccessLevel(ValueDecl *d);

public:
  /// Determine whether one type is a subtype of another.
  ///
  /// \param t1 The potential subtype.
  /// \param t2 The potential supertype.
  /// \param dc The context of the check.
  ///
  /// \returns true if \c t1 is a subtype of \c t2.
  bool IsSubTypeOf(Type t1, Type t2, DeclContext *dc);
};
} // namespace typecheck
} // namespace stone
#endif
