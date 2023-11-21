#ifndef STONE_COMPILE_COMPILER_H
#define STONE_COMPILE_COMPILER_H

#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/DiagnosticEngine.h"

#include <deque>

namespace stone {
class Compiler;
class ModuleDecl;
class CompilerExecution;

class Compiler final {

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

  ModuleDecl *mainModule = nullptr;

public:
  CompilerInvocation invocation;

public:
  Compiler();

public:
  void Setup();
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }

  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.RemoveConsumer(consumer);
  }

public:
  DiagnosticEngine &GetDiags() { return diags; }
  bool HasError() { return diags.HasError(); }
  SrcMgr &GetSrcMgr() { return srcMgr; }

  CompilerInvocation &GetInvocation() { return invocation; }
  std::unique_ptr<CompilerExecution> GetExecutionForAction(ActionKind kind);
  Status ExecuteAction(ActionKind kind);

public:
  // Module
  ModuleDecl *GetMainModule() const;
  void SetMainModule(ModuleDecl *mainModule);
  Status CreateSourceFilesForMainModule(
      ModuleDecl *mod, llvm::SmallVectorImpl<SourceFile *> &files) const;

  SourceFile *CreateSourceFileForMainModule(ModuleDecl *mainModule,
                                            SourceFileKind fileKind,
                                            unsigned bufferID,
                                            bool isMainBuffer = false) const;
  SourceFile *ComputeMainSourceFileForModule(ModuleDecl *mod) const;

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
