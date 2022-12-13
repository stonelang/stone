#ifndef STONE_COMPILE_MODULESYSTEM_H
#define STONE_COMPILE_MODULESYSTEM_H

#include "stone/Basic/Error.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/Status.h"
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

class CompilerInvocation;
class CompilerInstance;

// TODO: Move to Syntax
class ModuleSystem final {
  // TODO: We need built-in information
  friend CompilerInvocation;
  friend CompilerInstance;

  CompilerInvocation &invocation;
  syn::SyntaxContext &sc;
  /// This is the main module that will be created
  mutable syn::ModuleDecl *mainModule = nullptr;

  /// Contains \c MemoryBuffers for partial serialized module files and
  /// corresponding partial serialized module documentation files. This is
  /// \c mutable as it is consumed by \c loadPartialModulesAndImplicitImports.
  // mutable std::vector<ModuleBuffers> partialModules;

public:
  ModuleSystem(CompilerInvocation &invocation, syn::SyntaxContext &sc);
  ~ModuleSystem();

public:
  syn::ModuleDecl *GetMainModule() const;
  void SetMainModule(syn::ModuleDecl *mod);

  Error CreateSyntaxFilesForMainModule(
      syn::ModuleDecl *mod,
      llvm::SmallVectorImpl<syn::ModuleFile *> &files) const;

  syn::SyntaxFile *
  CreateSyntaxFileForMainModule(syn::ModuleDecl *mod,
                                syn::SyntaxFileKind fileKind, unsigned bufferID,
                                bool isMainBuffer = false) const;

  syn::SyntaxFile *ComputeMainSyntaxFileForModule(syn::ModuleDecl *mod) const;

  ModuleOptions &GetModuleOptions() { return GetCompilerOptions().moduleOpts; }
  const ModuleOptions &GetModuleOptions() const {
    return GetCompilerOptions().moduleOpts;
  }

  CompilerOptions &GetCompilerOptions();
  const CompilerOptions &GetCompilerOptions() const;

  syn::SyntaxFile::ParsingOptions
  GetSyntaxFileParsingOptions(bool forPrimary) const;

public:
  static Error IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
