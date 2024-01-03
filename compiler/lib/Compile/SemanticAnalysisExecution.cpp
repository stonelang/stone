#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Core.h"

using namespace stone;

TypeCheckExecution::TypeCheckExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status TypeCheckExecution::ExecuteAction() {

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

void TypeCheckExecution::CompletedSyntaxAnalysis(SourceFile &result) {}

void TypeCheckExecution::CompletedSyntaxAnalysis(ModuleDecl &result) {}

PrintASTExecution::PrintASTExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintASTExecution::ExecuteAction() {

  // assert(GetExecutionAction() == ActionKind::PrintAST);

  //  GetCompiler().ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
  //    stone::PrintSourceFile(sourceFile, GetCompiler().GetASTContext());
  //    return Status();
  //  });
  return Status();
}

void PrintASTExecution::CompletedSyntaxAnalysis(SourceFile &result) {}

void PrintASTExecution::CompletedSyntaxAnalysis(ModuleDecl &result) {}
