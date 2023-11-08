#ifndef STONE_GEN_CODEGENCONTEXT_H
#define STONE_GEN_CODEGENCONTEXT_H

#include "stone/AST/ASTContext.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/TargetOptions.h"

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
class Lang;
class Clang;
class CodeGenOptions;
class ModuleOptions;
class TargetOptions;


class CodeGenContext final {
private:
  Lang &lang;
  Clang &clangInstance;

  const CodeGenOptions &codeGenOpts;
  const ModuleOptions &moduleOpts;
  const stone::TargetOptions &targetOpts;

  ASTContext &astContext;
  llvm::LLVMContext &llvmContext;
  std::unique_ptr<llvm::Module> llvmModule;
  std::unique_ptr<llvm::TargetMachine> llvmTargetMachine;
  llvm::GlobalVariable **outModuleHash;

private:
  llvm::PassBuilder pb;
  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;
  llvm::ModulePassManager mpm;
  llvm::legacy::PassManager lpm;
  llvm::legacy::FunctionPassManager lfpm;
  llvm::FunctionPassManager fpm;

private:
  CodeGenContext(const CodeGenOptions &codeGenOpts,
                 const ModuleOptions &moduleOpts,
                 const stone::TargetOptions &targetOpts,
                 llvm::LLVMContext &llvmContext, ASTContext &astContext,
                 Lang &lang, Clang &clangInstance,
                 std::unique_ptr<llvm::Module> llvmMod,
                 llvm::GlobalVariable **outModuleHash = nullptr);

public:
  CodeGenContext(const CodeGenOptions &codeGenOpts,
                 const ModuleOptions &moduleOpts,
                 const stone::TargetOptions &targetOpts,
                 llvm::LLVMContext &llvmContext, ASTContext &astContext,
                 Lang &lang, Clang &clangInstance,
                 llvm::GlobalVariable **outModuleHash = nullptr);

public:
  CodeGenContext(const CodeGenContext &) = delete;
  void operator=(const CodeGenContext &) = delete;
  CodeGenContext(CodeGenContext &&) = delete;
  void operator=(CodeGenContext &&) = delete;

public:
  const llvm::Module *GetLLVMModule() const { return llvmModule.get(); }
  llvm::Module *GetLLVMModule() { return llvmModule.get(); }

  const llvm::TargetMachine &GetLLVMTargetMachine() const {
    return *llvmTargetMachine;
  }
  llvm::TargetMachine &GetLLVMTargetMachine() { return *llvmTargetMachine; }
  llvm::LLVMContext &GetLLVMContext() { return llvmContext; }
  ASTContext &GetASTContext() { return astContext; }

  Lang &GetLang() { return lang; }
  Clang &GetClang() { return clangInstance; }

  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }
  const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }

  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

  llvm::GlobalVariable **GetOutModuleHash() { outModuleHash; }

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