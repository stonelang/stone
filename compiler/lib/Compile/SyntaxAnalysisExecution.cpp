#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Core.h"

using namespace stone;

ParseExecution::ParseExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status ParseExecution::ExecuteAction() {

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

    // if (!IsSelfAction() && HasConsumer()) {
    //   GetConsumer()->CompletedSyntaxAnalysis(sourceFile);
    // }
    return Status();
  });

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedSyntaxAnalysis(compiler);
  }

  return Status();
}

ImportResolutionExecution::ImportResolutionExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status ImportResolutionExecution::ExecuteAction() {
  // assert(GetExecutionAction() == ActionKind::ResolveImports);

  CompilerStatsTracer tracer(&GetCompiler().GetStatsReporter(),
                             "import-resolution");

  // stone::ResolveSourceFileImports(sourceFile);

  return Status();
}

CompilerExecution *ImportResolutionExecution::GetConsumer() { return this; }

// void ImportResolutionExecution::CompletedSyntaxAnalysis(SourceFile& result)
// {}

DumpASTExecution::DumpASTExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status DumpASTExecution::ExecuteAction() {

  // stone::DumpSourceFile(sourceFile, compiler.GetASTContext());
  return Status();
}

CompilerExecution *DumpASTExecution::GetConsumer() { return this; }
