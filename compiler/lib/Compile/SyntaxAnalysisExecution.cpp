#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SyntaxAnalysisExecution::SyntaxAnalysisExecution(Compiler &compiler,
                                                 ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status SyntaxAnalysisExecution::Execute() {

  switch (GetExecutionAction()) {}

  // }
  // switch (compiler.GetInvocation().GetAction().GetKind()) {
  // case ActionKind::Parse:
  //   return ExecuteParse(
  //       [&](SourceFile &sourceFile) { return Status::Success(); });
  // case ActionKind::ResolveImports:
  //   return ExecuteParse([&](SourceFile &sourceFile) {
  //     return ExecuteParseAndResolveImports(sourceFile)
  //   });
  // case ActionKind::DumpSyntax:{
  //   return DumpSyntax();
  // }
  // default:
  //   llvm_unreachable("Invalid action for syntax analysis");
  // }
}

Status SyntaxAnalysisExecution::ExecuteParse(
    std::function<Status(SourceFile &sourceFile)> notify) {
  return Status();
}

Status
SyntaxAnalysisExecution::ExecuteParseAndResolveImports(SourceFile &sourceFile) {
  return Status();
}

Status SyntaxAnalysisExecution::ExecutDumpSyntax() { return Status(); }
