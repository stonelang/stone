#include "stone/Compile/CompilerInstance.h"

using namespace stone;

/// Handles only syntax
Status CompilerInstance::CompileForSemanticAnalysis() {}

Status CompilerInstance::CompileForTypeCheck(
    std::function<Status(syn::SyntaxFile &)> notifiy) {

  if (CompileForResolveImports().IsError()) {
  }
  return Status();
}

/// Handles semantics
Status CompilerInstance::CompileForPrintAST() {
  // CompileForTypeCheck();
}