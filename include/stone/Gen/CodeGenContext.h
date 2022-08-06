#ifndef STONE_GEN_CODEGENCONTEXT_H
#define STONE_GEN_CODEGENCONTEXT_H

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"
#include <memory>

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class Context;
class CodeGenOptions;

class CodeGenContext final {
  const Context &langContext;
  const CodeGenOptions &genOpts;
  llvm::LLVMContext &llvmContext;
  llvm::PassBuilder pb;
  // Create the analysis managers.
  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;
  llvm::ModulePassManager mpm;
  // legacy::PassManager legacyPM;
  std::unique_ptr<llvm::TargetMachine> targetMachine;

public:
  CodeGenContext(llvm::LLVMContext &llvmContext, const CodeGenOptions &genOpts,
                 const Context &langContext);
  ~CodeGenContext();

public:
  const CodeGenOptions &GetCodeGenOptions() const { return genOpts; }
  const Context &GetLangContext() const { return langContext; }

public:
  void TakeTargetMachine(std::unique_ptr<llvm::TargetMachine> &&tm) {
    targetMachine = std::move(tm);
  }

public:
  llvm::PassBuilder &GetPassBuilder() { return pb; }
  llvm::LoopAnalysisManager &GetLoopAnalysisManager() { return lam; }
  llvm::FunctionAnalysisManager &GetFunctionAnalysisManager() { return fam; }
  llvm::CGSCCAnalysisManager &GetCGSCCAnalysisManager() { return cgam; }
  llvm::ModuleAnalysisManager &GetModuleAnalysisManager() { return mam; }
  llvm::ModulePassManager &ModuleAnalysisManager() { return mpm; }
  llvm::TargetMachine &GetTargetMachine() { return *targetMachine; }
};
} // namespace stone

#endif
