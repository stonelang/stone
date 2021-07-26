#ifndef STONE_SEMANTICS_CHECKER_H
#define STONE_SEMANTICS_CHECKER_H

#include "stone/Basic/Basic.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/TreeContext.h"

#include <memory>

using namespace stone::syn;

namespace stone {
class TypeCheckerPipeline;

namespace syn {
class SyntaxFile;
} // namespace syn
namespace sema {
class TypeChecker;
class TypeCheckerOptions;

class TypeCheckerStats final : public Stats {
  const TypeChecker &checker;

public:
  TypeCheckerStats(const TypeChecker &checker, Basic &basic)
      : Stats("checker statistics:", basic), checker(checker) {}
  void Print() override;
};

class TypeCheckerDiagnostics {};

class TypeChecker final {
  friend TypeCheckerStats;
  syn::SyntaxFile &sf;
  TypeCheckerOptions &typeCheckerOpts;
  std::unique_ptr<TypeCheckerStats> stats;
  TypeCheckerPipeline *pipeline;

public:
  /// TODO: Pass in Syntax so that you can create the Nodes
  TypeChecker(syn::SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
              TypeCheckerPipeline *pipeline = nullptr);

public:
  void CheckDecl(Decl *d);
  void CheckFunDecl();

public:
  void CheckStmt();

public:
  void CheckExpr();
};
} // namespace sema
} // namespace stone
#endif
