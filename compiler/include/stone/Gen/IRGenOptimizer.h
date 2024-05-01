#ifndef STONE_GEN_CODEGEN_OPTIMIZER_H
#define STONE_GEN_CODEGEN_OPTIMIZER_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/AST/Diagnostics.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class TargetMachine;
} // namespace llvm

namespace stone {
class CodeGenOptions;

// There are two layers to optimization
// (1) optimization during module constructiong
// (2) optimization when actually generating backend code
// So, this is a convenient class to wrapp of the essential objects required.
class IRGenOptimizer final {
  const CodeGenOptions &codeGenOpts;
  llvm::Module *mod = nullptr;
  llvm::TargetMachine *targetMachine = nullptr;

  DiagnosticEngine &diags;

  llvm::PassBuilder pb;
  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;
  llvm::ModulePassManager mpm;
  llvm::legacy::PassManager lpm;
  llvm::legacy::FunctionPassManager lfpm;
  llvm::FunctionPassManager fpm;

public:
  IRGenOptimizer(const IRGenOptimizer &) = delete;
  void operator=(const IRGenOptimizer &) = delete;
  IRGenOptimizer(IRGenOptimizer &&) = delete;
  void operator=(IRGenOptimizer &&) = delete;

public:
  IRGenOptimizer(const CodeGenOptions &codeGenOpts, llvm::Module *mod,
                 llvm::TargetMachine *targetMachine, DiagnosticEngine &diags);
  ~IRGenOptimizer();

public:
  llvm::PassBuilder &GetPassBuilder() { return pb; }
  llvm::LoopAnalysisManager &GetLoopAnalysisManager() { return lam; }
  llvm::FunctionAnalysisManager &GetFunctionAnalysisManager() { return fam; }
  llvm::CGSCCAnalysisManager &GetCGSCCAnalysisManager() { return cgam; }
  llvm::ModuleAnalysisManager &GetModuleAnalysisManager() { return mam; }
  llvm::ModulePassManager &GetPassManager() { return mpm; }
  llvm::legacy::PassManager &GetLegacyPassManager() { return lpm; }
  llvm::legacy::FunctionPassManager &GetLegacyFunctionPassManager() {
    return lfpm;
  }
  llvm::FunctionPassManager &GetFunctionPassManager() { return fpm; }

public:
  llvm::TargetMachine *GetTargetMachine() { return targetMachine; }
  llvm::Module *GetLLVMModule() { return mod; }

public:
  void Optimize();
  void OptimizeWithLegacyPassManager();
};

} // namespace stone

#endif
