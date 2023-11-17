#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/DiagnosticEngine.h"

#include <deque>

namespace stone {

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
  // This is a simple queue to determine the order of the actions
  std::deque<ActionKind> actions;

  CompilerInvocation invocation;
  CompilerExecution execution;
  CompilerModule mainModule;

public:
  Compiler();

  CompilerInvocation &GetInvocation() { return invocation; }
  CompilerExecution &GetExecution() { return execution; }

public:
  void Setup();
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }

public:
  void SetupAction(ActionKind kind);
  void QueueAction(ActionKind kind);
  Status ForEachAction(std::function<Status(ActionKind kind)>);

public:
  DiagnosticEngine &GetDiags() { return diags; }
  bool HasError() { return diags.HasError(); }
  SrcMgr &GetSrcMgr() { return srcMgr; }

  CompilerModule &GetMainModule();

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
