#ifndef STONE_CHECK_CHECKER_H
#define STONE_CHECK_CHECKER_H

#include "stone/Core/Context.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"

#include <memory>

using namespace stone::syn;

namespace stone {
class TypeCheckerListener;

namespace syn {
class SyntaxFile;
} // namespace syn
namespace types {
class TypeChecker;
class TypeCheckerOptions;

class TypeCheckerStats final : public Stats {
  const TypeChecker &checker;

public:
  TypeCheckerStats(const TypeChecker &checker, Context &ctx)
      : Stats("checker statistics:", ctx), checker(checker) {}
  void Print() override;
};

class TypeCheckerDiagnostics {};

class TypeChecker final {
  friend TypeCheckerStats;
  syn::SyntaxFile &sf;
  TypeCheckerOptions &typeCheckerOpts;
  std::unique_ptr<TypeCheckerStats> stats;
  TypeCheckerListener *pipeline;

public:
  /// TODO: Pass in Syntax so that you can create the Nodes
  TypeChecker(syn::SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
              TypeCheckerListener *pipeline = nullptr);

public:
  void CheckDecl(Decl *d);
  void CheckFunDecl();

public:
  void CheckStmt();

public:
  void CheckExpr();
};
} // namespace types
} // namespace stone
#endif
