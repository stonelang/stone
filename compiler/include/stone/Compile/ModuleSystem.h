#ifndef STONE_COMPILE_MODULESYSTEM_H
#define STONE_COMPILE_MODULESYSTEM_H

#include "stone/Basic/Error.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/ModuleDependencies.h"
#include "stone/AST/Module.h"
#include "stone/AST/ASTContext.h"

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

// TODO: Move to AST
class ModuleSystem final {
  // TODO: We need built-in information
  CompilerInvocation &invocation;
  syn::ASTContext &sc;
  /// This is the main module that will be created
  mutable syn::ModuleDecl *mainModule = nullptr;

  /// Contains \c MemoryBuffers for partial serialized module files and
  /// corresponding partial serialized module documentation files. This is
  /// \c mutable as it is consumed by \c loadPartialModulesAndImplicitImports.
  // mutable std::vector<ModuleBuffers> partialModules;

public:
  ModuleSystem(CompilerInvocation &invocation, syn::ASTContext &sc);
  ~ModuleSystem();

public:
  syn::ModuleDecl *GetMainModule() const;
  void SetMainModule(syn::ModuleDecl *mod);

  Error CreateASTFilesForMainModule(
      syn::ModuleDecl *mod,
      llvm::SmallVectorImpl<syn::ModuleFile *> &files) const;

  syn::ASTFile *
  CreateASTFileForMainModule(syn::ModuleDecl *mod,
                                syn::ASTFileKind fileKind, unsigned bufferID,
                                bool isMainBuffer = false) const;

  syn::ASTFile *ComputeMainASTFileForModule(syn::ModuleDecl *mod) const;

  CompilerInvocation &GetCompilerInvocation() { return invocation; }

  syn::ASTFile::ParsingOptions
  GetASTFileParsingOptions(bool forPrimary) const;

public:
  static Error IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
