#ifndef STONE_GEN_CODEGENCONTEXT_H
#define STONE_GEN_CODEGENCONTEXT_H

#include "stone/Basic/CodeGenOptions.h"
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

class Lang;
class Clang;
class CodeGenOptions;
class ModuleOptions;
class TargetOptions;

namespace stone {
namespace codegen {

class CodeGenContext final {
private:
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
  CodeGenContext::CodeGenContext(
      const CodeGenOptions &genOpts, llvm::LLVMContext &llvmContext,
      const stone::TargetOptions &targetOpts, const Lang &lang,
      ASTContext &astContext, Clang &clang,
      std::unique_ptr<llvm::Module> llvmMod,
      llvm::GlobalVariable **outModuleHash = nullptr);

public:
  CodeGenContext(const CodeGenOptions &genOpts, llvm::LLVMContext &llvmContext,
                 const stone::TargetOptions &targetOpts, const Lang &lang,
                 ASTContext &astContext, Clang &clang,
                 llvm::GlobalVariable **outModuleHash = nullptr);

public:
  CodeGenContext(const CodeGenContext &) = delete;
  void operator=(const CodeGenContext &) = delete;
  CodeGenContext(CodeGenContext &&) = delete;
  void operator=(CodeGenContext &&) = delete;

public:
  const llvm::Module *GetLLVMModule() const { return llvmModule.get(); }
  llvm::Module *GetLLVMModule() { return llvmModule.get(); }

  const llvm::LLVMContext *GetLLVMContext() const { return llvmContext.get(); }
  llvm::LLVMContext *GetLLVMContext() { return llvmContext.get(); }

  const llvm::TargetMachine *GetLLVMTargetMachine() const {
    return llvmTargetMachine.get();
  }
  llvm::TargetMachine *GetLLVMTargetMachine() {
    return llvmTargetMachine.get();
  }

  LLVMContext &GetLLVMContext() { return llvmContext; }
  ASTContext &GetASTContext() { return astContext; }

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

} // namespace codegen
} // namespace stone