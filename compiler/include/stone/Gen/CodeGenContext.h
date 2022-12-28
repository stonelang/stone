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

  Safe<llvm::Module> mod;
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

  llvm::TargetMachine &GetTargetMachine() {
    assert(tm.get());
    return *tm;
  }

  llvm::CodeGenFileType GetCodeGenFileType() {

    return (GetCodeGenOptions().codeGenOutputKind ==
                    CodeGenOutputKind::NativeAssembly
                ? llvm::CGFT_AssemblyFile
                : llvm::CGFT_ObjectFile);
  }
};

} // namespace stone

#endif
