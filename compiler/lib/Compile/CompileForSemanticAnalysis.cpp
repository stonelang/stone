#include "stone/Compile/CompilerInstance.h"

using namespace stone;

/// Handles only syntax
Status CompilerInstance::CompileForSemanticAnalysis() {}

Status CompilerInstance::CompileForTypeCheck(
    std::function<Status(syn::SyntaxFile &)> notifiy) {

  if (CompileForResoveImports().IsError()) {
  }
  return Status();
}

/// Handles semantics
Status CompilerInstance::CompileForPrintAST() {
  // CompileForTypeCheck();
}