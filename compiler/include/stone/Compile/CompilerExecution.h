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

class PrintHelpExecution final : public CompilerExecution {
public:
  PrintHelpExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
};

class PrintVersionExecution final : public CompilerExecution {

public:
  PrintVersionExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
};

class PrintFeatureExecution final : public CompilerExecution {
public:
  PrintFeatureExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
};

class ParseOnlyExecution final : public CompilerExecution {
public:
  ParseOnlyExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
};
class ImportResolutionExecution final : public CompilerExecution {
public:
  ImportResolutionExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  ActionKind GetDependency() override { return ActionKind::Parse; }
};

class DumpASTExecution final : public CompilerExecution {
public:
  DumpASTExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  ActionKind GetDependency() override { return ActionKind::Parse; }
};

class TypeCheckExecution final : public CompilerExecution {
public:
  TypeCheckExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  ActionKind GetDependency() override { return ActionKind::ResolveImports; }
};

class PrintASTExecution final : public CompilerExecution {
public:
  PrintASTExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  ActionKind GetDependency() override { return ActionKind::TypeCheck; }
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
