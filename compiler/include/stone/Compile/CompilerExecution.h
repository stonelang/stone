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

  llvm::sys::TimePoint<> startTime;
  llvm::sys::TimePoint<> endTime = llvm::sys::TimePoint<>::min();

protected:
  Compiler &compiler;

public:
  CompilerExecution(Compiler &compiler);
  ~CompilerExecution();

public:
  virtual Status Setup();
  virtual Status Execute() = 0;

public:
  void Finish();

public:
  // Just one for now
  virtual ActionKind GetDependency() { return ActionKind::None; }
};

class SupportExecution final : public CompilerExecution {

public:
  SupportExecution(Compiler &compiler);

public:
  Status Execute() override;

public:
  Status ExecutePrintHelp();
  Status ExecutePrintVersion();
  Status ExecutePrintFeature();
};

class SyntaxAnalysisExecution final : public CompilerExecution {

public:
  SyntaxAnalysisExecution(Compiler &compiler);

public:
  Status Execute() override;

public:
  Status ExecuteParse(std::function<Status(SourceFile &)> notify);
  Status ExecuteParseAndResolveImports();
  Status ExecutDumpSyntax();

private:
  Status ExecuteParseAndResolveImports(SourceFile &sourceFile);

public:
  ActionKind GetDependency() override { return ActionKind::None; }
};

class SemanticAnalysisExecution final : public CompilerExecution {

public:
  SemanticAnalysisExecution(Compiler &compiler);

public:
  Status Execute() override;

public:
  Status ExecuteTypeCheck();
  Status ExecuteDumpTypeInfo();
  Status ExecutePrintSyntax();

public:
  ActionKind GetDependency() override { return ActionKind::ResolveImports; }
};

class CodeGenExecution final : public CompilerExecution {

public:
  CodeGenExecution(Compiler &compiler);

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
  FallbackExecution(Compiler &compiler);

public:
  Status Execute() override;
};

} // namespace stone

#endif
