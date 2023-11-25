#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Public.h"

using namespace stone;

ParseOnlyExecution::ParseOnlyExecution(Compiler &compiler,
                                       ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status ParseOnlyExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::Parse);
  GetCompiler().ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    stone::ParseSourceFile(sourceFile, GetCompiler().GetASTContext(), nullptr,
                           nullptr);
    sourceFile.stage = SourceFileStage::Parsed;
    return Status();
  });
  return Status();
}

ImportResolutionExecution::ImportResolutionExecution(Compiler &compiler,
                                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status ImportResolutionExecution::Execute() {
  assert(GetExecutionAction() == ActionKind::ResolveImports);
  // stone::ResolveSourceFileImports(sourceFile);
  return Status();
}

DumpASTExecution::DumpASTExecution(Compiler &compiler, ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status DumpASTExecution::Execute() {
  assert(GetExecutionAction() == ActionKind::DumpAST);
  // stone::DumpSourceFile(sourceFile, compiler.GetASTContext());
  return Status();
}
