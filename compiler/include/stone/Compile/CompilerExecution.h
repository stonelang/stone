#ifndef STONE_COMPILE_COMPILER_EXECUTION_H
#define STONE_COMPILE_COMPILER_EXECUTION_H

#include "stone/Gen/CodeGenContext.h"

#include "stone/Basic/Color.h"
#include "stone/Basic/Status.h"
#include "stone/Option/ActionKind.h"

#include "llvm/Support/Chrono.h"

namespace stone {

class Compiler;
class SourceFile;
class CodeGenContext;

class CompilerExecution {
  Compiler &compiler;

protected:
  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

  Status depStatus;
  ActionKind currentAction;

public:
  CompilerExecution(Compiler &compiler,
                    ActionKind currentAction = ActionKind::None);
  virtual ~CompilerExecution();

public:
  virtual Status Setup();
  virtual Status Execute() = 0;
  Status Finish();

private:
  void SetDependencyStatus(Status status) { depStatus = status; }

protected:
  // Just one for now
  virtual ActionKind GetDependency() { return ActionKind::None; }
  bool HasDependency() { return GetDependency() != ActionKind::None; }
  Status GetDependencyStatus() { return depStatus; }

protected:
  bool IsMainAction() { return GetCurrentAction() == GetMainAction(); }
  ActionKind GetMainAction();
  ActionKind GetCurrentAction() { return currentAction; }
  ActionKind GetExecutionAction() {
    return IsMainAction() ? GetMainAction() : GetCurrentAction();
  }
  Compiler &GetCompiler() { return compiler; }
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

class IRGeneration {
  Compiler &compiler;

protected:
  // This is the main result returned from generation
  // You want to create these internally
  std::unique_ptr<CodeGenContext> codeGenContext;

public:
  IRGeneration(Compiler &compiler);

private:
  Status GenForSourceFile();
  Status GenForWholeModule();

protected:
  Status GenerateIR();

protected:
  CodeGenContext &GetCodeGenContext() { return *codeGenContext; }
};

class IROptimization {
public:
  IROptimization(Compiler &compiler);

protected:
  Status OptimizeIR();
};

// Generate IR, then print it.
class EmitIRBeforeExecution final : public CompilerExecution,
                                    public IRGeneration {
public:
  EmitIRBeforeExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
  ActionKind GetDependency() override { return ActionKind::TypeCheck; }
};

// Generate IR, optimize ir, then print it.
class EmitIRAfterExecution final : public CompilerExecution,
                                   public IRGeneration {
public:
  EmitIRAfterExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
  ActionKind GetDependency() override { return ActionKind::TypeCheck; }
};

class EmitBitCodeExecution final : public CompilerExecution,
                                   public IRGeneration {
public:
  EmitBitCodeExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
  ActionKind GetDependency() override { return ActionKind::TypeCheck; }
};

class EmitModuleExecution final : public CompilerExecution, IRGeneration {
public:
  EmitModuleExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  ActionKind GetDependency() override { return ActionKind::TypeCheck; }
};

class EmitNativeExecution : public CompilerExecution, public IRGeneration {
public:
  EmitNativeExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
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
