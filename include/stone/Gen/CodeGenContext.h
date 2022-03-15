
#ifndef STONE_GEN_CODEGENCONTEXT_H
#define STONE_GEN_CODEGENCONTEXT_H

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

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

class CodeGenOptions;

class CodeGenContext final {
  const CodeGenOptions &genOpts;
  llvm::LLVMContext &llvmContext;

  llvm::PassBuilder pb;
  // Create the analysis managers.
  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;
  llvm::ModulePassManager mpm;

  std::unique_ptr<llvm::Module> llvmModule;

public:
  CodeGenContext(llvm::LLVMContext &llvmContext, const CodeGenOptions &genOpts);
  ~CodeGenContext();

public:
  const CodeGenOptions &GetCodeGenOptions() const { return genOpts; }

  llvm::Module *GetLLVMModule() { return llvmModule.get(); }
  llvm::Module *ReleaseLLVMModule() { return llvmModule.release(); }

public:
};
} // namespace stone

#endif
