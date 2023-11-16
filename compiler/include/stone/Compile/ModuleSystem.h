#ifndef STONE_COMPILE_MODULESYSTEM_H
#define STONE_COMPILE_MODULESYSTEM_H

#include "stone/Basic/ModuleOptions.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Compile/ModuleDependencies.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

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

class CompilerConfiguration;

// TODO: Move to Syntax
class ModuleSystem final {
  // TODO: We need built-in information
  Compiler &compiler;
  syn::ASTContext &syntaxContext;
  /// This is the main module that will be created
  mutable syn::ModuleDecl *mainModule = nullptr;

  /// Contains \c MemoryBuffers for partial serialized module files and
  /// corresponding partial serialized module documentation files. This is
  /// \c mutable as it is consumed by \c loadPartialModulesAndImplicitImports.
  // mutable std::vector<ModuleBuffers> partialModules;

public:
  ModuleSystem(Compiler &compiler, syn::ASTContext &syntaxContext);
  ~ModuleSystem();

public:
  syn::ModuleDecl *GetMainModule() const;
  void SetMainModule(syn::ModuleDecl *mod);

  Status CreateASTFilesForMainModule(
      syn::ModuleDecl *mod,
      llvm::SmallVectorImpl<syn::ModuleFile *> &files) const;

  syn::ASTFile *CreateASTFileForMainModule(syn::ModuleDecl *mod,
                                           syn::ASTFileKind fileKind,
                                           unsigned bufferID,
                                           bool isMainBuffer = false) const;

  syn::ASTFile *ComputeMainASTFileForModule(syn::ModuleDecl *mod) const;

  Compiler &GetCompiler() { return compiler; }

  syn::ASTFile::ParsingOptions GetASTFileParsingOptions(bool forPrimary) const;

public:
  static Status IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
