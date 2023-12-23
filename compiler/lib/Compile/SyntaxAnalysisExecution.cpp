#include "stone/Basic/Status.h"
#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Core.h"

using namespace stone;

ParseOnlyExecution::ParseOnlyExecution(Compiler &compiler,
                                       ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status ParseOnlyExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::Parse);

  CompilerStatsTracer tracer(&GetCompiler().GetStatsReporter(),
                             "parse-source-file");

  CodeCompletionCallbacks *codeCompletionCallbacks = nullptr;
  if (compiler.HasObservation()) {
    codeCompletionCallbacks =
        compiler.GetObservation()->GetCodeCompletionCallbacks();
  }

  compiler.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    stone::ParseSourceFile(sourceFile, GetCompiler().GetASTContext(),
                           codeCompletionCallbacks);
    sourceFile.stage = SourceFileStage::Parsed;
    if (codeCompletionCallbacks) {
      codeCompletionCallbacks->CompletedParseSourceFile(&sourceFile);
    }
    return Status();
  });

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedSyntaxAnalysis(compiler);
  }

  return Status();
}

ImportResolutionExecution::ImportResolutionExecution(Compiler &compiler,
                                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status ImportResolutionExecution::Execute() {
  assert(GetExecutionAction() == ActionKind::ResolveImports);

  CompilerStatsTracer tracer(&GetCompiler().GetStatsReporter(),
                             "import-resolution");

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
