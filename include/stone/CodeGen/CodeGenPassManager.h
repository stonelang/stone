#ifndef STONE_CODEGEN_CODEGENPASSMGR_H
#define STONE_CODEGEN_CODEGENPASSMGR_H

#include "stone/Basic/CodeGenOptions.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

namespace stone {

// There are two layers to optimization
// (1) optimization during module constructiong
// (2) optimization when actually generating backend code
// So, this is a convenient class to wrapp of the essential objects required.
class CodeGenPassManager final {
  const CodeGenOptions &codeGenOpts;
  llvm::Module *llvmModule = nullptr;

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
  CodeGenPassManager(const CodeGenPassManager &) = delete;
  void operator=(const CodeGenPassManager &) = delete;
  CodeGenPassManager(CodeGenPassManager &&) = delete;
  void operator=(CodeGenPassManager &&) = delete;

public:
  CodeGenPassManager(const CodeGenOptions &codeGenOpts,
                     llvm::Module *llvmModule);
  ~CodeGenPassManager();

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
  llvm::Module *GetLLVMModule() { return llvmModule; }
  const CodeGenOptions &GetCodGenOptions() const { return codeGenOpts; }

public:
  void RunLegacyPasses();
  void RunPasses();
};

} // namespace stone

#endif
