#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SyntaxAnalysisExecution::SyntaxAnalysisExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status SyntaxAnalysisExecution::Execute() {

  switch (compiler.GetInvocation().GetAction().GetKind()) {
  case ActionKind::Parse:
    return ExecuteParse(
        [&](SourceFile &sourceFile) { return Status::Success(); });
  case ActionKind::ResolveImports:
    return ExecuteParseAndResolveImports();
  default:
    llvm_unreachable("Invalid action for syntax analysis");
  }
}

Status SyntaxAnalysisExecution::ExecuteParse(
    std::function<Status(SourceFile &sourceFile)> notify) {
  return Status();
}

Status SyntaxAnalysisExecution::ExecuteParseAndResolveImports() {
  return ExecuteParse([&](SourceFile &sourceFile) {
    return ExecuteParseAndResolveImports(sourceFile);
  });
}

Status
SyntaxAnalysisExecution::ExecuteParseAndResolveImports(SourceFile &sourceFile) {
}

Status SyntaxAnalysisExecution::ExecutDumpSyntax() { return Status(); }
