#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SyntaxAnalysisExecution::SyntaxAnalysisExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status SyntaxAnalysisExecution::Setup() {}

Status SyntaxAnalysisExecution::Execute() {

  // switch (compiler.GetInvocation().GetAction().GetKind()) {
  // case ActionKind::Parse:
  //   return ExecuteParseOnly();
  // case ActionKind::ResolveImports:
  //   return ExecuteParseAndResolveImports();
  // default:
  //   llvm_unreachable("Invalid action for syntax analysis");
  // }
}

Status SyntaxAnalysisExecution::ExecuteParseOnly() { return Status(); }

Status SyntaxAnalysisExecution::ExecuteParseAndResolveImports() {
  // if (ExecuteParseOnly().IsError()) {
  //   return Status::Error();
  // }
}
Status SyntaxAnalysisExecution::ExecutDumpSyntax() { return Status(); }
