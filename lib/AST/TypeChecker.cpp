#include "stone/AST/TypeChecker.h"

using namespace stone;

void TypeChecker::CheckSourceFile(SourceFile &sourceFile) {

  for (auto D : sourceFile.GetTopLevelDecls()) {
    TypeChecker::CheckDecl(D);
    sourceFile.SetTypeCheckedStage();
  }
}
