#ifndef STONE_SEM_TYPECHECKER_H
#define STONE_SEM_TYPECHECKER_H

#include "stone/Basic/StatisticEngine.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

#include <memory>

namespace stone {
class TypeCheckerOptions;
class TypeCheckerListener;

class SourceFile;
class Expr;
class Decl;
class Stmt;

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

  ASTContext &astContext;
  TypeCheckerOptions &typeCheckerOpts;
  std::unique_ptr<TypeCheckerStats> stats;
  TypeCheckerListener *listener;

public:
  TypeChecker(const TypeChecker &) = delete;
  TypeChecker &operator=(const TypeChecker &) = delete;
  ~TypeChecker();

public:
  /// TODO: Pass in Syntax so that you can create the Nodes
  TypeChecker(ASTContext &astContext, TypeCheckerOptions &typeCheckerOpts,
              TypeCheckerListener *listener = nullptr);

public:
  void CheckASTNode(ASTNode &syntaxNode, DeclContext *dc,
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

} // namespace stone
#endif
