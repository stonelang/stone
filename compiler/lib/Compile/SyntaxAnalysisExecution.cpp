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
    return ExecuteDumpAST();
  case ActionKind::ResolveImports:
    return ExecuteResolveImports();
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

Status SyntaxAnalysisExecution::ExecuteDumpAST() {

  return ExecuteParse([&](SourceFile &sourceFile) {
    stone::DumpSourceFile(sourceFile, compiler.GetASTContext());
    return Status();
  });
}

Status SyntaxAnalysisExecution::ExecuteResolveImports() {
  // TODO: Additional pre work
  return ExecuteParse([&](SourceFile &sourceFile) {
    stone::ResolveSourceFileImports(sourceFile);
    return Status();
  });
}
