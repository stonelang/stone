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

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

public:
  CompilerInvocation invocation;
  CompilerModule mainModule;

public:
  Compiler();

public:
  void Setup();
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }

public:
  DiagnosticEngine &GetDiags() { return diags; }
  bool HasError() { return diags.HasError(); }
  SrcMgr &GetSrcMgr() { return srcMgr; }

  CompilerInvocation &GetInvocation() { return invocation; }
  std::unique_ptr<CompilerExecution> GetExecutionForAction(ActionKind kind);
  Status ExecuteAction(ActionKind kind);

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
