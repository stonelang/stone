#ifndef STONE_SEM_TYPECHECKER_H
#define STONE_SEM_TYPECHECKER_H

#include "stone/Basic/StatisticEngine.h"
#include "stone/Context.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"

#include <memory>

using namespace stone::syn;

namespace stone {
class TypeCheckerListener;

namespace syn {
class SyntaxFile;
} // namespace syn
namespace sem {
class TypeChecker;
class TypeCheckerOptions;

class TypeCheckerStats final : public Stats {
  const TypeChecker &checker;

public:
  TypeCheckerStats(const TypeChecker &checker)
      : Stats("checker statistics:"), checker(checker) {}
  void Print(ColorfulStream &stream) override;
};

class TypeCheckerDiagnostics {};

class TypeChecker final {
  friend TypeCheckerStats;

  SyntaxContext &sc;
  TypeCheckerOptions &typeCheckerOpts;
  std::unique_ptr<TypeCheckerStats> stats;
  TypeCheckerListener *pipeline;

public:
  /// TODO: Pass in Syntax so that you can create the Nodes
  TypeChecker(SyntaxContext &sc, TypeCheckerOptions &typeCheckerOpts,
              TypeCheckerListener *pipeline = nullptr);

public:
  void CheckDecl(Decl *d);
  void CheckFunDecl(Decl *d);

  // DeclVisitor &GetDeclVisitor();

public:
  void CheckStmt();

  // StmtVisitor& GetStmtVisitor();

public:
  void CheckExpr();
};
} // namespace sem
} // namespace stone
#endif
