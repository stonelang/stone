#ifndef STONE_COMPILE_COMPILEREXECUTION_H
#define STONE_COMPILE_COMPILEREXECUTION_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Option/ActionKind.h"

namespace stone {

class Compiler;
struct CompilerExecutionFlags final {
  CompilerExecutionFlags() = delete;
  enum Kind : unsigned {
    None = 0,

    StartedParseOnly = 1 << 1,
    CompletedParseOnly = 1 << 2,

    StartedSyntaxAnalysis = 1 << 3,
    CompletedSyntaxAnalysis = 1 << 4,

    StartedSemanticAnalysis = 1 << 5,
    CompletedSemanticAnalysis = 1 << 6,
  };
};

class CompilerExecutionStages final {
  UInt32 stages = 0;

public:
  CompilerExecutionStages() : stages(0) {}

public:
  bool HasStartedParseOnly() const {
    return stages & CompilerExecutionFlags::StartedParseOnly;
  }
  void AddStartedParseOnly() {
    stages |= CompilerExecutionFlags::StartedParseOnly;
  }
  bool HasCompletedParseOnly() const {
    return stages & CompilerExecutionFlags::CompletedParseOnly;
  }
  void AddCompletedParseOnly() {
    stages |= CompilerExecutionFlags::CompletedParseOnly;
  }

public:
  bool HasStartedSyntaxAnalysis() const {
    return stages & CompilerExecutionFlags::StartedSyntaxAnalysis;
  }
  void AddStartedSyntaxAnalysis() {
    stages |= CompilerExecutionFlags::StartedSyntaxAnalysis;
  }
  bool HasCompletedSyntaxAnalysis() const {
    return stages & CompilerExecutionFlags::CompletedSyntaxAnalysis;
  }
  void AddCompletedSyntaxAnalysis() {
    stages |= CompilerExecutionFlags::CompletedSyntaxAnalysis;
  }

public:
  bool HasStartedSemanticAnalysis() const {
    return stages & CompilerExecutionFlags::StartedSemanticAnalysis;
  }
  void AddStartedSemanticAnalysis() {
    stages |= CompilerExecutionFlags::StartedSemanticAnalysis;
  }

  bool HasCompletedSemanticAnalysis() const {
    return stages & CompilerExecutionFlags::CompletedSemanticAnalysis;
  }
  void AddCompletedSemanticAnalysis() {
    stages |= CompilerExecutionFlags::CompletedSemanticAnalysis;
  }
};

class CompilerExecution final {
  Compiler &compiler;

  ActionKind currentAction;

  CompilerExecutionStages stages;

  std::unique_ptr<CodeGenContext> codeGenContext;

public:
  CompilerExecutionStages &GetStages() { return stages; }

public:
  CompilerExecution(Compiler &compiler);
  void Setup();

  Status ExecuteAction();
  Status ExecuteAction(ActionKind action);

public:
  Status ExecutePrintHelp();
  Status ExecutePrintVersion();
  Status ExecutePrintFeature();

public:
  Status ExecuteParseOnly();
  Status ExecuteResolveImports();
  Status ExecuteDumpSyntax();

public:
  Status ExecuteTypeCheck();
  Status ExecuteDumpTypeInfo();
  Status ExecutePrintSyntax();
  Status WithCompletedTypeChecking(std::function<Status()> notify);

  Status ExecutePrintIR();
  Status ExecuteEmitIRBefore();

public:
  Status WithGenerateIR(std::function<Status()> notify);
  Status ExecuteInitModule();
  Status ExecuteEmitModule();
  Status ExecuteMergeModules();

public:
  Status ExecuteEmitIRAfter();
  Status ExecuteEmitBC();
  Status ExecuteEmitObject();
  Status ExecuteEmitLibrary();
  Status ExecuteEmitAssembly();
};

class CompilerExecutionRAII final {
  CompilerExecution &execution;

public:
  CompilerExecutionRAII(CompilerExecution &execution);
  ~CompilerExecutionRAII();
};

} // namespace stone
#endif
