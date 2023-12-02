#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

TypeCheckExecution::TypeCheckExecution(Compiler &compiler,
                                       ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status TypeCheckExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::TypeCheck);

  GetCompiler().ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    stone::TypeCheckSourceFile(
        sourceFile, GetCompiler().GetInvocation().GetTypeCheckerOptions(),
        nullptr);
    sourceFile.stage = SourceFileStage::TypeChecked;
    return Status();
  });
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
