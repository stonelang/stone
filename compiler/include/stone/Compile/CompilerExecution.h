#ifndef STONE_COMPILE_COMPILEREXECUTION_H
#define STONE_COMPILE_COMPILEREXECUTION_H

#include "stone/Basic/Color.h"
#include "stone/Basic/Status.h"
#include "stone/Option/ActionKind.h"

#include "llvm/Support/Chrono.h"

namespace stone {

class Compiler;
class SourceFile;
class CodeGenContext;

class CompilerExecution {

protected:
  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

  Compiler &compiler;
  ActionKind currentAction;

public:
  CompilerExecution(Compiler &compiler,
                    ActionKind currentAction = ActionKind::None);
  virtual ~CompilerExecution();

public:
  virtual Status Setup();
  virtual Status Execute() = 0;
  Status Finish();

protected:
  // Just one for now
  virtual ActionKind GetDependency() { return ActionKind::None; }
  bool IsMainAction() { return GetCurrentAction() == GetMainAction(); }
  ActionKind GetMainAction();
  ActionKind GetCurrentAction() { return currentAction; }
  ActionKind GetExecutionAction() {
    return IsMainAction() ? GetMainAction() : GetCurrentAction();
  }
};

class SupportExecution final : public CompilerExecution {

public:
  SupportExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  Status ExecutePrintHelp();
  Status ExecutePrintVersion();
  Status ExecutePrintFeature();
};

class SyntaxAnalysisExecution final : public CompilerExecution {
public:
  SyntaxAnalysisExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  Status ExecuteParse(std::function<Status(SourceFile &)> notify = nullptr);
  Status ExecuteDumpAST();
  Status ExecuteResolveImports();
};

class SemanticAnalysisExecution final : public CompilerExecution {

  // using EachSourceFileTypeCheckedCallback = std::function<void(
  //     SourceFile &, TypeCheckerOptions &, TypeCheckerListener *)>;

public:
  SemanticAnalysisExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  Status ExecuteTypeCheck();
  Status ExecutePrintAST();

public:
  ActionKind GetDependency() override { return ActionKind::ResolveImports; }
};

class CodeGenExecution final : public CompilerExecution {

public:
  CodeGenExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  Status ExecuteGenIR(CodeGenContext &codeGenContext);
  Status ExecuteGenNative(CodeGenContext &codeGenContext);

public:
  ActionKind GetDependency() override { return ActionKind::TypeCheck; }
};

class FallbackExecution final : public CompilerExecution {

public:
  FallbackExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  Status ExecuteCompileLLVMIR();
};

} // namespace stone

#endif
