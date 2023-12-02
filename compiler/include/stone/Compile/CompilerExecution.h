#ifndef STONE_COMPILE_COMPILER_EXECUTION_H
#define STONE_COMPILE_COMPILER_EXECUTION_H

#include "stone/Basic/Color.h"
#include "stone/Basic/Status.h"
#include "stone/Option/ActionKind.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Chrono.h"

namespace stone {

class Compiler;
class SourceFile;

/* :public Compiler*/
class CompilerExecution {
  Compiler &compiler;

protected:
  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

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
  bool HasDependency() { return GetDependency() != ActionKind::None; }
  // virtual llvm::ArrayRef<ActionKind> GetDependencies() { return
  // {ActionKind::None}; }

protected:
  bool IsMainAction() { return GetCurrentAction() == GetMainAction(); }
  ActionKind GetMainAction();
  ActionKind GetCurrentAction() { return currentAction; }
  ActionKind GetExecutionAction() {
    return IsMainAction() ? GetMainAction() : GetCurrentAction();
  }

public:
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

// Generate IR, before optimization
class GenerateIRExecution final : public CompilerExecution {
  // llvm::GlobalVariable *hashGlobal;

  /// IRCodeGen

public:
  GenerateIRExecution(Compiler &compiler, ActionKind currentAction);

private:
  Status GenForFile();
  Status GenForModule();
  bool IsForModule();

public:
  Status Execute() override;
  ActionKind GetDependency() override { return ActionKind::TypeCheck; }
};

// Generate IR, optimize ir, then print it.
class OptimizeIRExecution final : public CompilerExecution {

  // IRCodeOptimizer;
public:
  OptimizeIRExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
  ActionKind GetDependency() override { return ActionKind::EmitIRBefore; }
};

class EmitBitCodeExecution final : public CompilerExecution {
public:
  EmitBitCodeExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
  ActionKind GetDependency() override { return ActionKind::EmitIRAfter; }
};

class EmitModuleExecution final : public CompilerExecution {
public:
  EmitModuleExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;

public:
  ActionKind GetDependency() override { return ActionKind::EmitIRAfter; }
};

class EmitNativeExecution : public CompilerExecution {

  /// NativeCodeGen
public:
  EmitNativeExecution(Compiler &compiler, ActionKind currentAction);

public:
  Status Execute() override;
  ActionKind GetDependency() override { return ActionKind::EmitIRAfter; }
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
