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

    StartParseOnly = 1 << 1,
    FininsParseOnly = 1 << 2,

    StartSyntaxAnalysis = 1 << 3,
    FinishSyntaxAnalysis = 1 << 4,

    StartTypeChecking = 1 << 5,
    FinishTypeChecking = 1 << 6,

    StartGeneratingIR = 1 << 7,
    FinishGeneratingIR = 1 << 8,

    StartGeneratingNative = 1 << 9,
    FinishGeneratingNative = 1 << 10,
  };
};

class CompilerExecutionStages final {
  UInt32 stages = 0;

public:
  CompilerExecutionStages() : stages(0) {}

public:
  bool DidStartParseOnly() const {
    return stages & CompilerExecutionFlags::StartParseOnly;
  }
  void StartParseOnly() { stages |= CompilerExecutionFlags::StartParseOnly; }
  bool DidFinishParseOnly() const {
    return stages & CompilerExecutionFlags::FininsParseOnly;
  }
  void FinishParseOnly() { stages |= CompilerExecutionFlags::FininsParseOnly; }

public:
  bool DidStartSyntaxAnalysis() const {
    return stages & CompilerExecutionFlags::StartSyntaxAnalysis;
  }
  void StartSyntaxAnalysis() {
    stages |= CompilerExecutionFlags::StartSyntaxAnalysis;
  }
  bool DidFinishSyntaxAnalysis() const {
    return stages & CompilerExecutionFlags::FinishSyntaxAnalysis;
  }
  void FinishSyntaxAnalysis() {
    stages |= CompilerExecutionFlags::FinishSyntaxAnalysis;
  }

public:
  bool DidStartTypeChecking() const {
    return stages & CompilerExecutionFlags::StartTypeChecking;
  }
  void StartTypeChecking() {
    stages |= CompilerExecutionFlags::StartTypeChecking;
  }

  bool DidFinishTypeChecking() const {
    return stages & CompilerExecutionFlags::FinishTypeChecking;
  }
  void FinishTypeChecking() {
    stages |= CompilerExecutionFlags::FinishTypeChecking;
  }

public:
  bool DidStartGeneratingIR() const {
    return stages & CompilerExecutionFlags::StartGeneratingIR;
  }
  void StartGeneratingIR() {
    stages |= CompilerExecutionFlags::StartGeneratingIR;
  }

  bool DidFinishGeneratingIR() const {
    return stages & CompilerExecutionFlags::FinishGeneratingIR;
  }
  void FinishGeneratingIR() {
    stages |= CompilerExecutionFlags::FinishGeneratingIR;
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
  Status VerifyInputFiles();
  Status ExecuteParseOnly();
  Status ExecuteResolveImports();
  Status ExecuteDumpSyntax();

public:
  Status ExecuteTypeCheck();
  Status ExecuteDumpTypeInfo();
  Status ExecutePrintSyntax();
  Status WithCompletedTypeChecking();

public:
  Status GenerateIR(CodeGenContext &cgc);
  Status ExecuteInitModule();
  
  Status ExecuteMergeModules();
  Status ExecuteEmitModule(CodeGenContext &cgc);

public:
  Status ExecuteEmitIRAfter(CodeGenContext &cgc);
  Status ExecutePrintIR(CodeGenContext &cgc);
  Status ExecuteEmitIRBefore(CodeGenContext &cgc);
  Status ExecuteEmitBC(CodeGenContext &cgc);
  Status ExecuteEmitObject(CodeGenContext &cgc);
  Status ExecuteEmitLibrary(CodeGenContext &cgc);
  Status ExecuteEmitAssembly(CodeGenContext &cgc);
};

class CompilerExecutionRAII final {
  CompilerExecution &execution;

public:
  CompilerExecutionRAII(CompilerExecution &execution);
  ~CompilerExecutionRAII();
};

} // namespace stone
#endif
