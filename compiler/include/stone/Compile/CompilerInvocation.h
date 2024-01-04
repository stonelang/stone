#ifndef STONE_COMPILE_COMPILERINVOCATION_H
#define STONE_COMPILE_COMPILERINVOCATION_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Diag/DiagnosticOptions.h"
#include "stone/Syntax/ASTOptions.h"
#include "stone/Syntax/TypeCheckerOptions.h"

#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"
#include "stone/Basic/Status.h"
#include "stone/Basic/TargetOptions.h"
#include "stone/Syntax/ClangContext.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SearchPath.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"

#include <memory>

namespace stone {

class Compiler;

using ConfigurationFileBuffers =
    llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>;

struct ModuleBuffers {

  std::unique_ptr<llvm::MemoryBuffer> moduleBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer;

  // Constructor
  ModuleBuffers(
      std::unique_ptr<llvm::MemoryBuffer> moduleBuffer,
      std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer = nullptr,
      std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer = nullptr)
      : moduleBuffer(std::move(moduleBuffer)),
        moduleDocBuffer(std::move(moduleDocBuffer)),
        moduleSourceInfoBuffer(std::move(moduleSourceInfoBuffer)) {}
};

using MemoryBuffers =
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>>;

class CompilerInvocation final {
  Compiler &compiler;

  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  LangOptions langOpts;

  ASTOptions astOpts;

  DiagnosticOptions diagOpts;

  FileSystemOptions fileSystemOpts;

  TargetOptions targetOptions;

  std::unique_ptr<ClangContext> clangContext;

public:
  CompilerInvocation(Compiler &compiler);

public:
  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  stone::TargetOptions &GetTargetOptions() { return targetOptions; }
  const stone::TargetOptions &GetTargetContext() const { return targetOptions; }

  ASTOptions &GetASTOptions() { return astOpts; }
  const ASTOptions &GetASTOptions() const { return astOpts; }

  TypeCheckerOptions &GetTypeCheckerOptions() { return typeCheckerOpts; }
  const TypeCheckerOptions &GetTypeCheckerOptions() const {
    return typeCheckerOpts;
  }
  SearchPathOptions &GetSearchPathOptions() { return searchPathOpts; }
  const SearchPathOptions &GetSearchPathOptions() const {
    return searchPathOpts;
  }

  DiagnosticOptions &GetDiagnosticOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagnosticOptions() const { return diagOpts; }

  FileSystemOptions &GetFileSystemOptions() { return fileSystemOpts; }
  const FileSystemOptions &GetFileSystemOptions() const {
    return fileSystemOpts;
  }
  void SetTargetTriple(llvm::StringRef triple);

public:
  Status SetupClang(llvm::ArrayRef<const char *> args, const char *arg0);
  ClangContext &GetClangContext() { return *clangContext; }

public:
  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForAtMostOnePrimary() const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForPrimary(StringRef fileName) const;

  const PrimaryFileSpecificPaths &
  GetPrimaryFileSpecificPathsForSyntaxFile(const SourceFile &sf) const;

public:
  Status ParseCommandLine(llvm::ArrayRef<const char *> args);
};

} // namespace stone
#endif
