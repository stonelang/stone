#ifndef STONE_GEN_CODEGENCONTEXT_H
#define STONE_GEN_CODEGENCONTEXT_H

#include "stone/Basic/Error.h"
#include "stone/Basic/Mem.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class LangContext;
class ClangContext;
class CodeGenOptions;
class ModuleOptions;

class CodeGenContext final {
  llvm::LLVMContext &llvmContext;
  const CodeGenOptions &genOpts;
  const ModuleOptions &moduleOpts;

  const LangContext &langContext;
  ClangContext &clangContext;

  llvm::PassBuilder pb;
  // Create the analysis managers.
  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;
  llvm::ModulePassManager mpm;

  mem::Safe<llvm::Module> mod;
  // legacy::PassManager legacyPM;
  mem::Safe<llvm::TargetMachine> tm;

public:
  CodeGenContext(llvm::LLVMContext &llvmContext, const CodeGenOptions &genOpts,
                 const ModuleOptions &moduleOpts,
                 const LangContext &langContext, ClangContext &clangContext);
  ~CodeGenContext();

public:
  const CodeGenOptions &GetCodeGenOptions() const { return genOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }
  const LangContext &GetLangContext() const { return langContext; }
  llvm::LLVMContext &GetLLVMContext() const { return llvmContext; }

  ClangContext &GetClangContext() { return clangContext; }

  const llvm::Module &GetLLVMModule() const { return *mod; }
  llvm::Module &GetLLVMModule() { return *mod; }

public:
  llvm::PassBuilder &GetPassBuilder() { return pb; }
  llvm::LoopAnalysisManager &GetLoopAnalysisManager() { return lam; }
  llvm::FunctionAnalysisManager &GetFunctionAnalysisManager() { return fam; }
  llvm::CGSCCAnalysisManager &GetCGSCCAnalysisManager() { return cgam; }
  llvm::ModuleAnalysisManager &GetModuleAnalysisManager() { return mam; }
  llvm::ModulePassManager &ModuleAnalysisManager() { return mpm; }
  llvm::TargetMachine &GetTargetMachine() { return *tm; }

private:
  mem::Safe<llvm::TargetMachine> CreateTarget(ClangContext &clangContext);
};
} // namespace stone

#endif
