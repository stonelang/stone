#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Public.h"

using namespace stone;

SyntaxAnalysisExecution::SyntaxAnalysisExecution(Compiler &compiler,
                                                 ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status SyntaxAnalysisExecution::Execute() {

  switch (GetExecutionAction()) {
  case ActionKind::Parse:
    return ExecuteParse(
        [&](SourceFile &sourceFile) { return Status::Success(); });
  case ActionKind::DumpAST:
    return ExecuteParse(
        [&](SourceFile &sourceFile) { return ExecutDumpAST(sourceFile); });
  case ActionKind::ResolveImports:
    return ExecuteParse([&](SourceFile &sourceFile) {
      return ExecuteResolveImports(sourceFile);
    });
  default:
    llvm_unreachable("Invalid action for syntax analysis");
  }
}

Status SyntaxAnalysisExecution::ExecuteParse(
    std::function<Status(SourceFile &sourceFile)> notify) {
  compiler.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    stone::ParseSourceFile(sourceFile, compiler.GetASTContext(), nullptr,
                           nullptr);
    if (notify(sourceFile).IsError()) {
      return Status::Error();
    }
    return Status();
  });

  return Status();
}

Status SyntaxAnalysisExecution::ExecutDumpAST(SourceFile &sourceFile) {

  return Status();
}

Status SyntaxAnalysisExecution::ExecuteResolveImports(SourceFile &sourceFile) {

  // TODO: Simple for now -- may just call directly
  stone::ResolveSourceFileImports(sourceFile);

  return Status();
}
