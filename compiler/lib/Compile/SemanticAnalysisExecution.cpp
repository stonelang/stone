#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SemanticAnalysisExecution::SemanticAnalysisExecution(Compiler &compiler,
                                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status SemanticAnalysisExecution::Execute() {

  switch (GetExecutionAction()) {
  case ActionKind::TypeCheck:
    return ExecuteTypeCheck();
  case ActionKind::PrintAST:
    return ExecutePrintAST();
  default:
    llvm_unreachable("Invalid action for semantic analysis");
  }
}

Status SemanticAnalysisExecution::ExecuteTypeCheck(
    std::function<Status(SourceFile &)> notify) {

  compiler.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    stone::TypeCheckSourceFile(
        sourceFile, compiler.GetInvocation().GetTypeCheckerOptions(), nullptr);
    if (notify) {
      if (notify(sourceFile).IsError()) {
        return Status::Error();
      }
    }
    return Status();
  });
}

Status SemanticAnalysisExecution::ExecutePrintAST() {

  return ExecuteTypeCheck([&](SourceFile &sourceFile) {
    stone::PrintSourceFile(sourceFile, compiler.GetASTContext());
    return Status();
  });
  return Status();
}
