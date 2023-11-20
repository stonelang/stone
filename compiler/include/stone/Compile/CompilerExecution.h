#ifndef STONE_COMPILE_COMPILEREXECUTION_H
#define STONE_COMPILE_COMPILEREXECUTION_H

#include "stone/Basic/Color.h"
#include "stone/Basic/Status.h"

#include "llvm/Support/Chrono.h"

namespace stone {

class Compiler;
class SourceFile;
class CodeGenContext;

// enum class CompilerExecutionKind {
//   BeforeCodeAnalysis = 0,
//   CodeAnalysis,
//   AfterCodeAnalysis,
//   CodeGen,
// };

class CompilerExecution {

  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

protected:
  Compiler &compiler;

public:
  CompilerExecution(Compiler &compiler);

public:
  virtual Status Setup() = 0;
  virtual Status Execute() = 0;
};

class SupportExecution final : public CompilerExecution {

public:
  SupportExecution(Compiler &compiler);

public:
  Status Setup() override;
  Status Execute() override;

public:
  Status ExecutePrintHelp();
  Status ExecutePrintVersion();
  Status ExecutePrintFeature();
};

class BeforeCodeAnalysisExecution final : public CompilerExecution {

public:
  BeforeAnalysisExecution(Compiler &compiler);

public:
  Status Setup() override;
  Status Execute() override;
};

class SyntaxAnalysisExecution final : public CompilerExecution {

public:
  SyntaxAnalysisExecution(Compiler &compiler);

public:
  Status Setup() override;
  Status Execute() override;

public:
  Status ExecuteParse(std::function<Status(SourceFile &)> notify);
  Status ExecuteParseAndResolveImports();
  Status ExecutDumpSyntax();

private:
  Status ExecuteParseAndResolveImports(SourceFile &sourceFile);
};

class SemanticAnalysisExecution final : public CompilerExecution {

public:
  SemanticAnalysisExecution(Compiler &compiler);

public:
  Status Setup() override;
  Status Execute() override;

public:
  Status ExecuteTypeCheck();
  Status ExecuteDumpTypeInfo();
  Status ExecutePrintSyntax();
};

class AfterCodeAnalysisExecution final : public CompilerExecution {
public:
  AfterCodeAnalysisExecution(Compiler &compiler);

public:
  Status Setup() override;
  Status Execute() override;
};

class CodeGenExecution final : public CompilerExecution {

public:
  CodeGenExecution(Compiler &compiler);

public:
  Status Setup() override;
  Status Execute() override;

public:
  Status ExecuteGenIR(CodeGenContext &codeGenContext);
  Status ExecuteGenNative(CodeGenContext &codeGenContext);
};

} // namespace stone

#endif
