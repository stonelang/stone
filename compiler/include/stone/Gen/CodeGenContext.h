#ifndef STONE_GEN_CODEGENCONTEXT_H
#define STONE_GEN_CODEGENCONTEXT_H

#include "stone/Basic/Error.h"
#include "stone/Basic/STDAlias.h"

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
class TargetOptions;

class CodeGenContext final {
  llvm::LLVMContext &llvmContext;
  const CodeGenOptions &genOpts;
  const ModuleOptions &moduleOpts;
  const stone::TargetOptions &targetOpts;

  const LangContext &langContext;
  ClangContext &clangContext;

  llvm::PassBuilder pb;
  // Create the analysis managers.
  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;
  llvm::ModulePassManager mpm;

  Safe<llvm::Module> mod;
  // legacy::PassManager legacyPM;
  Safe<llvm::TargetMachine> tm;

  llvm::GlobalVariable **outModuleHash;

public:
  CodeGenContext(llvm::LLVMContext &llvmContext, const CodeGenOptions &genOpts,
                 const ModuleOptions &moduleOpts,
                 const stone::TargetOptions &targetOpts,
                 const LangContext &langContext, ClangContext &clangContext,
                 llvm::GlobalVariable **outModuleHash = nullptr);
  ~CodeGenContext();

public:
  const CodeGenOptions &GetCodeGenOptions() const { return genOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }
  const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }
  const LangContext &GetLangContext() const { return langContext; }
  llvm::LLVMContext &GetLLVMContext() const { return llvmContext; }

  ClangContext &GetClangContext() { return clangContext; }

  const llvm::Module &GetLLVMModule() const {
    assert(mod.get());
    return *mod;
  }
  llvm::Module &GetLLVMModule() {
    assert(mod.get());
    return *mod;
  }

public:
  llvm::PassBuilder &GetPassBuilder() { return pb; }
  llvm::LoopAnalysisManager &GetLoopAnalysisManager() { return lam; }
  llvm::FunctionAnalysisManager &GetFunctionAnalysisManager() { return fam; }
  llvm::CGSCCAnalysisManager &GetCGSCCAnalysisManager() { return cgam; }
  llvm::ModuleAnalysisManager &GetModuleAnalysisManager() { return mam; }
  llvm::ModulePassManager &ModuleAnalysisManager() { return mpm; }

  llvm::TargetMachine &GetTargetMachine() {
    assert(tm.get());
    return *tm;
  }

private:
  Safe<llvm::TargetMachine> CreateTargetMachine();
};
} // namespace stone

#endif
