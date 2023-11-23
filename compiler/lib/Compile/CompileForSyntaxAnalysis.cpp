#include "stone/Compile/CompilerInstance.h"

using namespace stone;

/// Handles only syntax
Status CompilerInstance::CompileForParse(
    std::function<Status(syn::SyntaxFile &)> notify) {

	for (auto moduleFile : GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *syntaxFile = llvm::dyn_cast<syn::SyntaxFile>(moduleFile)) {
      stone::ParseSyntaxFile(*syntaxFile, GetSyntaxContext(),
                             invocation.GetListener());
      if (notify) {
        if(notify(*syntaxFile).IsError()){
        	return Status::Error();
        }
      }
    }
  }
  return Status();
}

/// Handles only syntax
Status CompilerInstance::CompileForDumpAST() { return Status(); }

/// Handles only syntax
Status CompilerInstance::CompileForResolveImports() { return Status(); }
