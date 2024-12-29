#include "stone/AST/TypeChecker.h"
#include "stone/Compile/CompilerAction.h"
#include "stone/Compile/CompilerInstance.h"

using namespace stone;

bool TypeCheckAction::ExecuteAction() {

  FrontendStatsTracer actionTracer(instance.GetStats(),
                                   GetSelfActionKindString());

  auto TypeCheckSourceFile = [&](SourceFile &sourceFile,
                                 TypeCheckerOptions &typeCheckerOpts) -> bool {
    assert(sourceFile.HasParsed() &&
           "Unable to type-check a source-file that was not parsed.");
    return TypeChecker(sourceFile, typeCheckerOpts).CheckTopLevelDecls();
  };

  instance.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    if (!TypeCheckSourceFile(
            sourceFile, instance.GetInvocation().GetTypeCheckerOptions())) {
      return false;
    }
  });
  if (HasConsumer()) {
    GetConsumer()->DepCompleted(this);
  }
  return true;
}

void TypeCheckAction::DepCompleted(CompilerAction *dep) {}