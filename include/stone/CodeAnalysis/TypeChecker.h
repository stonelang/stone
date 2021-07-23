#ifndef STONE_CODEANALYSIS_CHECKER_H
#define STONE_CODEANALYSIS_CHECKER_H

#include "stone/Basic/Basic.h"
#include "stone/Basic/Stats.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/TreeContext.h"

#include <memory>

using namespace stone::syn;

namespace stone {
class TypeCheckerPipeline;

namespace syn {
class Syntax;
class SyntaxFile;
} // namespace syn
namespace sema {
class TypeChecker;

class CheckerStats final : public Stats {
  const TypeChecker &checker;

public:
  CheckerStats(const TypeChecker &checker, Basic &basic)
      : Stats("checker statistics:", basic), checker(checker) {}
  void Print() override;
};

class TypeCheckingDiagnostics {};

class TypeChecker final {
  friend CheckerStats;

  Syntax &syntax;
  syn::SyntaxFile &sf;
  std::unique_ptr<CheckerStats> stats;
  TypeCheckerPipeline *pipeline;

public:
  /// TODO: Pass in Syntax so that you can create the Nodes
  TypeChecker(syn::SyntaxFile &sf, Syntax &syntax,
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
