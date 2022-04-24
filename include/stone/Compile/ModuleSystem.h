#ifndef STONE_COMPILE_MODULESYSTEM_H
#define STONE_COMPILE_MODULESYSTEM_H

#include "stone/Basic/ModuleOptions.h"
#include "stone/Compile/FrontendOptions.h"
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
  // TODO: We need built-in information
  friend class Frontend;

  Context &ctx;
  syn::Syntax &syntax;

  ModuleOptions moduleOpts;

  /// This is the main module that will be created
  mutable syn::Module *mainModule = nullptr;

public:
  ModuleSystem(syn::Syntax &syntax, Context &ctx);
  ~ModuleSystem();

public:
  syn::Module *GetMainModule() const;
  const llvm::StringRef GetModuleName() const;
  void SetModuleName(llvm::StringRef name);

  ModuleOptions &GetModuleOptions() { return moduleOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

public:
  static stone::Error IsValidModuleName(llvm::StringRef moduleName);
};

} // namespace stone
#endif
