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
  ast::ASTContext &sc;
  /// This is the main module that will be created
  mutable ast::ModuleDecl *mainModule = nullptr;

  /// Contains \c MemoryBuffers for partial serialized module files and
  /// corresponding partial serialized module documentation files. This is
  /// \c mutable as it is consumed by \c loadPartialModulesAndImplicitImports.
  // mutable std::vector<ModuleBuffers> partialModules;

public:
  ModuleSystem(CompilerInvocation &invocation, ast::ASTContext &sc);
  ~ModuleSystem();

public:
  ast::ModuleDecl *GetMainModule() const;
  void SetMainModule(ast::ModuleDecl *mod);

  Error CreateASTFilesForMainModule(
      ast::ModuleDecl *mod,
      llvm::SmallVectorImpl<ast::ModuleFile *> &files) const;

  ast::ASTFile *
  CreateASTFileForMainModule(ast::ModuleDecl *mod,
                                ast::ASTFileKind fileKind, unsigned bufferID,
                                bool isMainBuffer = false) const;

  ast::ASTFile *ComputeMainASTFileForModule(ast::ModuleDecl *mod) const;

  CompilerInvocation &GetCompilerInvocation() { return invocation; }

  ast::ASTFile::ParsingOptions
  GetASTFileParsingOptions(bool forPrimary) const;

public:
  static Error IsValidModuleName(const llvm::StringRef moduleName);
};

} // namespace stone
#endif
