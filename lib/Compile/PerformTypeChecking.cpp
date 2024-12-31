#include "stone/AST/TypeChecker.h"
#include "stone/Compile/CompilerAction.h"
#include "stone/Compile/CompilerInstance.h"

using namespace stone;

bool TypeCheckAction::ExecuteAction() {

  FrontendStatsTracer actionTracer(instance.GetStats(),
                                   GetSelfActionKindString());

  instance.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    assert(sourceFile.HasParsed() &&
           "Unable to type-check a source-file that was not parsed.");
    if (!TypeChecker(sourceFile,
                     instance.GetInvocation().GetTypeCheckerOptions())
             .CheckTopLevelDecls()) {
      return false;
    }
  });
  if (HasConsumer()) {
    GetConsumer()->DepCompleted(this);
  }
  return true;
}

void TypeCheckAction::DepCompleted(CompilerAction *dep) {}