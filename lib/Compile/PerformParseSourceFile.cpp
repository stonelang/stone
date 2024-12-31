#include "stone/AST/TypeChecker.h"
#include "stone/Compile/CompilerAction.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Parse/CodeCompletionCallbacks.h"
#include "stone/Parse/Parser.h"

using namespace stone;

bool ParseAction::ExecuteAction() {

  FrontendStatsTracer actionTracer(instance.GetStats(),
                                   GetSelfActionKindString());

  instance.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    if (!Parser(sourceFile, instance.GetASTContext()).ParseTopLevelDecls()) {
      return false;
    }
    sourceFile.SetParsedStage();
    if (instance.HasObservation()) {
      auto codeCompletionCallbacks =
          instance.GetObservation()->GetCodeCompletionCallbacks();
      if (codeCompletionCallbacks) {
        codeCompletionCallbacks->CompletedParseSourceFile(&sourceFile);
      }
    }
  });
  if (HasConsumer()) {
    GetConsumer()->DepCompleted(this);
  }
  return true;
}
// bool CompilerInstance::EmitParseAction::ExecuteAction() {
//   FrontendStatsTracer actionTracer(instance.GetStats(),
//                                    GetSelfActionKindString());

//   return true;
// }

bool ResolveImportsAction::ExecuteAction() {

  FrontendStatsTracer actionTracer(instance.GetStats(),
                                   GetSelfActionKindString());

  auto PeformResolveImports = [&](CompilerInstance &instance,
                                  SourceFile &sourceFile) -> bool {
    return true;
  };

  instance.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    if (!PeformResolveImports(instance, sourceFile)) {
      return false;
    }
  });
  return true;
}
void ResolveImportsAction::DepCompleted(CompilerAction *action) {}