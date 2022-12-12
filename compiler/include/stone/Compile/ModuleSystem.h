#ifndef STONE_COMPILE_MODULESYSTEM_H
#define STONE_COMPILE_MODULESYSTEM_H

#include "stone/Basic/ModuleOptions.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/ModuleDependencies.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"

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

// TODO: Move to Syntax
class ModuleSystem final {
  // TODO: We need built-in information
  friend class CompilerInvocation;
  friend class CompilerInstance;

  LangContext &lc;
  syn::SyntaxContext &sc;
  ModuleOptions &moduleOpts;

  /// This is the main module that will be created
  mutable syn::ModuleDecl *mainModule = nullptr;

  /// Contains \c MemoryBuffers for partial serialized module files and
  /// corresponding partial serialized module documentation files. This is
  /// \c mutable as it is consumed by \c loadPartialModulesAndImplicitImports.
  // mutable std::vector<ModuleBuffers> partialModules;

public:
  ModuleSystem(LangContext &ctx, syn::SyntaxContext &sc,
               ModuleOptions &moduleOpts);
  ~ModuleSystem();

public:
  syn::ModuleDecl *GetMainModule() const;
  void SetMainModule(syn::ModuleDecl *mod);

  bool CreateSyntaxFilesForMainModule(
      syn::ModuleDecl *mod,
      llvm::SmallVectorImpl<syn::ModuleFile *> &files) const;

  syn::ModuleFile *ComputeMainSyntaxFileForModule(syn::ModuleDecl *mod) const;

  ModuleOptions &GetModuleOptions() { return moduleOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }

public:
  static stone::Error IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
