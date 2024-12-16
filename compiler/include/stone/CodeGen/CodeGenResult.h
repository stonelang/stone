#ifndef STONE_CODEGEN_CODEGENRESULT_H
#define STONE_CODEGEN_CODEGENRESULT_H
 
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
} // namespace stone

#endif
