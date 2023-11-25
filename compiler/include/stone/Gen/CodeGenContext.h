#ifndef STONE_GEN_CODEGENCONTEXT_H
#define STONE_GEN_CODEGENCONTEXT_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
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

class ASTContext;
class ClangContext;
class CodeGenOptions;
class ModuleOptions;
class TargetOptions;
class CodeGenListener;

class CodeGenContext final {

  const CodeGenOptions &genOpts;
  const ModuleOptions &moduleOpts;
  const TargetOptions &targetOptions;

  ASTContext &astContext;
  ClangContext &clangContext;

  llvm::LLVMContext &llvmContext;
  std::unique_ptr<llvm::Module> llvmModule;
  std::unique_ptr<llvm::TargetMachine> llvmTargetMachine;

  llvm::GlobalVariable *outModuleHash;
  CodeGenListener *listener;

public:
  CodeGenContext(const CodeGenOptions &genOpts, const ModuleOptions &moduleOpts,
                 const TargetOptions &targetOptions, ASTContext &astContext,
                 ClangContext &clangContext, llvm::LLVMContext &llvmContext,
                 llvm::GlobalVariable *outModuleHash = nullptr);

  ~CodeGenContext();

public:
  const CodeGenOptions &GetCodeGenOptions() const { return genOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }
  const TargetOptions &GetTargetOptions() const { return targetOptions; }
  ASTContext &GetASTContext() const { return astContext; }
  ClangContext &GetClangContext() { return clangContext; }

  const llvm::Module *GetLLVMModule() const { return llvmModule.get(); }
  llvm::Module *GetLLVMModule() { return llvmModule.get(); }

  const llvm::LLVMContext &GetLLVMContext() const { return llvmContext; }
  llvm::LLVMContext &GetLLVMContext() { return llvmContext; }

  const llvm::TargetMachine *GetLLVMTargetMachine() const {
    return llvmTargetMachine.get();
  }
  llvm::TargetMachine *GetLLVMTargetMachine() {
    return llvmTargetMachine.get();
  }
  llvm::GlobalVariable *GetOutModuleHash() { return outModuleHash; }

  void SetCodeGenListener(CodeGenListener *listener) { listener = listener; }
  CodeGenListener *GetCodeGenListener() { return listener; }

  llvm::CodeGenFileType GetCodeGenFileType() {
    return (GetCodeGenOptions().codeGenOutputKind ==
                    CodeGenOutputKind::NativeAssembly
                ? llvm::CGFT_AssemblyFile
                : llvm::CGFT_ObjectFile);
  }
};

} // namespace stone

#endif
