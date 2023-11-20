#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/DiagnosticEngine.h"

#include <deque>

namespace stone {
class ModuleDecl;
class CompilerExecution;

class CompilerModule final {
  ModuleDecl *modPtr = nullptr;

public:
  CompilerModule(ModuleDecl *modPtr = nullptr) : modPtr(modPtr) {}

public:
  bool IsNull() const { return modPtr == nullptr; }
  ModuleDecl *GetPtr() const { return modPtr; }

  ModuleDecl *operator->() const {
    assert(modPtr && "Cannot dereference a null ModuleDecl!");
    return modPtr;
  }
  explicit operator bool() const { return modPtr != nullptr; }

public:
  void AddSourceFiles();
  void AddSourceFile();
};

class Compiler final {

  friend CompilerExecution;

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

public:
  CompilerInvocation invocation;
  std::unique_ptr<CompilerExecution> execution;
  CompilerModule mainModule;

public:
  Compiler();

  CompilerInvocation &GetInvocation() { return invocation; }
  CompilerExecution &GetExecution() { return *execution; }

public:
  void Setup();
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }

public:
  DiagnosticEngine &GetDiags() { return diags; }
  bool HasError() { return diags.HasError(); }
  SrcMgr &GetSrcMgr() { return srcMgr; }

private:
  std::unique_ptr<CompilerExecution> GetExecution(ActionKind kind);

public:
  // void SetupMainStage();
  //  CompilerModule &GetMainModule();

public:
  // CompilerStage &GetMainStage { return *mainCompilerStage; }

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
