#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SemanticAnalysisExecution::SemanticAnalysisExecution(Compiler &compiler,
                                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status SemanticAnalysisExecution::Execute() {

  switch (GetExecutionAction()) {}
}

Status SemanticAnalysisExecution::ExecuteTypeCheck(
    std::function<Status(SourceFile &)> notify) {

  compiler.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    stone::TypeCheckSourceFile(
        sourceFile, compiler.GetInvocation().GetTypeCheckerOptions(), nullptr);
    return Status();
  });
}

Status SemanticAnalysisExecution::ExecutePrintAST() { return Status(); }