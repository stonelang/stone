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
  std::unique_ptr<ASTContext> astContext;
  mutable ModuleDecl *mainModule = nullptr;
  CompilerInvocation invocation;

public:
  Compiler(const Compiler &) = delete;
  void operator=(const Compiler &) = delete;
  Compiler(Compiler &&) = delete;
  void operator=(Compiler &&) = delete;
  Compiler();
  void Setup();

public:
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

  ASTContext &GetASTContext() { return *astContext; }
  const ASTContext &GetASTContext() const { return *astContext; }
  bool HasASTContext() const { return astContext != nullptr; }

  CompilerInvocation &GetInvocation() { return invocation; }
  std::unique_ptr<CompilerExecution> GetExecutionForAction(ActionKind kind);
  Status ExecuteAction(ActionKind kind);

public:
  // Module
  ModuleDecl *GetMainModule() const;
  void SetMainModule(ModuleDecl *mainModule);
  Status CreateSourceFilesForMainModule(
      ModuleDecl *mod, llvm::SmallVectorImpl<ModuleFile *> &files) const;

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
