#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Core.h"

using namespace stone;

TypeCheckExecution::TypeCheckExecution(Compiler &compiler,
                                       ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status TypeCheckExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::TypeCheck);

  CompilerStatsTracer tracer(&GetCompiler().GetStatsReporter(), "type-check");

  compiler.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    stone::TypeCheckSourceFile(
        sourceFile, GetCompiler().GetInvocation().GetTypeCheckerOptions());
    sourceFile.stage = SourceFileStage::TypeChecked;
    return Status();
  });

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedSemanticAnalysis(compiler);
  }

  return Status();
}

PrintASTExecution::PrintASTExecution(Compiler &compiler,
                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status PrintASTExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::PrintAST);

  //  GetCompiler().ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
  //    stone::PrintSourceFile(sourceFile, GetCompiler().GetASTContext());
  //    return Status();
  //  });
  return Status();
}
