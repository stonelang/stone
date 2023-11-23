#include "stone/Compile/CompilerInstance.h"

using namespace stone;

/// Handles only syntax
Status CompilerInstance::CompileForParse(
    std::function<Status(syn::SyntaxFile &)> notify) {
  return Status();
}

/// Handles only syntax
Status CompilerInstance::CompileForDumpAST() { return Status(); }

/// Handles only syntax
Status CompilerInstance::CompileForResolveImports() { return Status(); }
