#ifndef STONE_COMPILE_MODULESYSTEM_H
#define STONE_COMPILE_MODULESYSTEM_H

#include "stone/Compile/LangOptions.h"
#include "stone/Compile/ModuleDependencies.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Syntax.h"

namespace stone {

class ExternalDependencyTracker {
public:
  /// Adds a file as a dependency.
  ///
  /// The contents of \p File are taken literally, and should be appropriate
  /// for appearing in a list of dependencies suitable for tooling like Make.
  /// No path canonicalization is done.
  void AddDep(llvm::StringRef depFile, bool isSystem);
};
class ModuleSystem final {
  // We need built-in information
  syn::Syntax &syntax;

  const LangOptions &langOpts;
  /// This is the main module that will be created
  mutable syn::Module *mainModule = nullptr;

public:
  ModuleSystem(syn::Syntax &syntax, const LangOptions &langOpts);

  ~ModuleSystem();

  syn::Module *GetMainModule() const;

public:
  const llvm::StringRef GetModuleName() const {
    return langOpts.systemOpts.moduleName;
  }
};

} // namespace stone
#endif
