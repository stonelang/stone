#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

TypeCheckExecution::TypeCheckExecution(Compiler &compiler,
                                       ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status TypeCheckExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::TypeCheck);
  compiler.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    stone::TypeCheckSourceFile(
        sourceFile, compiler.GetInvocation().GetTypeCheckerOptions(), nullptr);
    return Status();
  });
  return Status();
}

PrintASTExecution::PrintASTExecution(Compiler &compiler,
                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status PrintASTExecution::Execute() {
  assert(GetExecutionAction() == ActionKind::PrintAST);
  compiler.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    stone::PrintSourceFile(sourceFile, compiler.GetASTContext());
    return Status();
  });
  return Status();
}
