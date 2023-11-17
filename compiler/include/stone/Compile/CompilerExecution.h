#ifndef STONE_COMPILE_COMPILEREXECUTION_H
#define STONE_COMPILE_COMPILEREXECUTION_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Option/ActionKind.h"

namespace stone {

class Compiler;
struct CompilerExecutionFlags final {
  CompilerExecutionFlags() = delete;
  enum Kind : unsigned {
    None = 0,
    StartedSyntaxAnalysis = 1 << 1,
    CompletedSyntaxAnalysis = 1 << 2,
    StartedSemanticAnalysis = 1 << 3,
    CompletedSemanticAnalysis = 1 << 4,
  };
};

class CompilerExecutionStages final {
  UInt32 stages = 0;

public:
  CompilerExecutionStages() : stages(0) {}

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

  // std::unique_ptr<CodeGenContext> codeGenContext;

public:
  CompilerExecutionStages &GetStages() { return stages; }

public:
  CompilerExecution(Compiler &compiler) : compiler(compiler) {}
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
  Status ExecutePrintIR();
  Status ExecuteEmitIRBefore();

public:
  Status ExecuteGenerateIR();
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
