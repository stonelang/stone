#ifndef STONE_CODEGEN_CODEGEN_H
#define STONE_CODEGEN_CODEGEN_H

#include "stone/AST/Diagnostics.h"
#include "stone/AST/Module.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"
#include "stone/Basic/Status.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class TargetMachine;
} // namespace llvm

namespace stone {

class CodeGenOptions;
class CodeGenModule;

class CodeGenResult final {

  std::unique_ptr<llvm::LLVMContext> llvmContext;
  std::unique_ptr<llvm::Module> llvmModule;
  std::unique_ptr<llvm::TargetMachine> llvmTargetMachine;

  CodeGenResult()
      : llvmContext(nullptr), llvmModule(nullptr), llvmTargetMachine(nullptr) {}

public:
  CodeGenResult(CodeGenResult const &) = delete;
  CodeGenResult &operator=(CodeGenResult const &) = delete;
  CodeGenResult(CodeGenResult &&) = default;
  CodeGenResult &operator=(CodeGenResult &&) = default;

public:
  /// Construct a \c CodeGenResult  that owns a given module and context.
  ///
  /// The given pointers must not be null. If a null \c CodeGenResult  is
  /// needed, use \c CodeGenResult ::null() instead.
  explicit CodeGenResult(std::unique_ptr<llvm::LLVMContext> &&llvmContext,
                         std::unique_ptr<llvm::Module> &&llvmModule,
                         std::unique_ptr<llvm::TargetMachine> &&Target)
      : llvmContext(std::move(llvmContext)), llvmModule(std::move(llvmModule)),
        llvmTargetMachine(std::move(llvmTargetMachine)) {}

public:
  explicit operator bool() const {
    return llvmModule != nullptr && llvmContext != nullptr;
  }

public:
  const llvm::Module *GetLLVMModule() const { return llvmModule.get(); }
  llvm::Module *GetLLVMModule() { return llvmModule.get(); }

  const llvm::LLVMContext *GetLLVMContext() const { return llvmContext.get(); }
  llvm::LLVMContext *GetLLVMContext() { return llvmContext.get(); }

  const llvm::TargetMachine *GetTargetMachine() const {
    return llvmTargetMachine.get();
  }
  llvm::TargetMachine *GetTargetMachine() { return llvmTargetMachine.get(); }

public:
  /// Release ownership of the context and module to the caller, consuming
  /// this value in the process.
  ///
  /// The REPL is the only caller that needs this. New uses of this function
  /// should be avoided at all costs.
  std::pair<llvm::LLVMContext *, llvm::Module *> release() && {
    return {llvmContext.release(), llvmModule.release()};
  };
};

class CodeGen final {

  const CodeGenOptions &codeGenOpts;
  ASTContext &astContext;
  llvm::DenseMap<SourceFile *, CodeGenModule *> modules;
  llvm::SmallVector<CodeGenModule *, 8> moduleQueue;

public:
  std::unique_ptr<llvm::LLVMContext> llvmContext;
  std::unique_ptr<llvm::TargetMachine> llvmTargetMachine;

public:
  CodeGen(const CodeGenOptions &codeGenOpts, ASTContext &astContext);

public:
  ASTContext &GetASTContext() { return astContext; }
  llvm::LLVMContext &GetLLVMContext() { return *llvmContext; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

public:
  ///\return the target machine
  llvm::TargetMachine &GetTargetMachine() { return *llvmTargetMachine; }

  ///\return the target machine this runs on.
  std::unique_ptr<llvm::TargetMachine> CreateTargetMachine();

  /// Return the effective triple used by clang.
  llvm::Triple GetEffectiveClangTriple();

  ///\return the clang datalayout string
  const llvm::StringRef GetClangDataLayoutString();

public:
  /// Add an CodeGenModule for a source file.
  /// Should only be called from CodeGenModule's constructor.
  void AddCodeGenModule(SourceFile *sourceFile, CodeGenModule *cgm);

  /// Add an CodeGenModule to the queue
  /// Should only be called from IRGenModule's constructor.
  void QueueCodeGenModule(SourceFile *sourceFile, CodeGenModule *cgm);

  /// Get an CodeGenModule for a declaration context.
  /// Returns the CodeGenModule of the containing source file, or if this
  /// cannot be determined, returns the primary CodeGenModule.
  CodeGenModule *GetCodeGenModule();

private:
};

} // namespace stone

#endif
