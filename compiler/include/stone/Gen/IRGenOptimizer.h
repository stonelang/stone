#ifndef STONE_GEN_CODEGEN_OPTIMIZER_H
#define STONE_GEN_CODEGEN_OPTIMIZER_H

#include "stone/AST/Diagnostics.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

namespace stone {
class IRGenInvocation;

// There are two layers to optimization
// (1) optimization during module constructiong
// (2) optimization when actually generating backend code
// So, this is a convenient class to wrapp of the essential objects required.
class IRGenOptimizer final {

  IRGenInvocation& invocation;
public:
  IRGenOptimizer(const IRGenOptimizer &) = delete;
  void operator=(const IRGenOptimizer &) = delete;
  IRGenOptimizer(IRGenOptimizer &&) = delete;
  void operator=(IRGenOptimizer &&) = delete;

public:
  IRGenOptimizer(IRGenInvocation& invocation);
  ~IRGenOptimizer();

public:
  void Optimize();
  void OptimizeWithLegacyPassManager();
};

} // namespace stone

#endif
