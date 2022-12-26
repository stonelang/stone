#ifndef STONE_GEN_CODEGENSCOPE_H
#define STONE_GEN_CODEGENSCOPE_H

#include "stone/Basic/Error.h"
#include "stone/Basic/STDAlias.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class TargetMachine;
} // namespace llvm

namespace stone {
class CodeGenOptions;

class CodeGenScope final {
  const CodeGenOptions &codeGenOpts;

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
  CodeGenScope(const CodeGenScope &) = delete;
  void operator=(const CodeGenScope &) = delete;
  CodeGenScope(CodeGenScope &&) = delete;
  void operator=(CodeGenScope &&) = delete;

public:
  CodeGenScope(const CodeGenOptions &codeGenOpts, llvm::Module *mod = nullptr);
  ~CodeGenScope();

public:
  llvm::PassBuilder &GetPassBuilder() { return pb; }
  llvm::LoopAnalysisManager &GetLoopAnalysisManager() { return lam; }
  llvm::FunctionAnalysisManager &GetFunctionAnalysisManager() { return fam; }
  llvm::CGSCCAnalysisManager &GetCGSCCAnalysisManager() { return cgam; }
  llvm::ModuleAnalysisManager &GetModuleAnalysisManager() { return mam; }
  llvm::ModulePassManager &ModulePassManager() { return mpm; }
  llvm::legacy::PassManager &GetLegacyPassManager() { return lpm; }
  llvm::legacy::FunctionPassManager &GetLegacyFunctionPassManager() {
    return lfpm;
  }
  llvm::FunctionPassManager &GetFunctionPassManager() { return fpm; }
};

} // namespace stone

#endif
