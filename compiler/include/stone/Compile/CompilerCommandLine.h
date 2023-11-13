#ifndef STONE_COMPILE_COMPILERCOMMANDLINE_H
#define STONE_COMPILE_COMPILERCOMMANDLINE_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Diag/DiagnosticOptions.h"
#include "stone/Syntax/SyntaxOptions.h"
#include "stone/Syntax/TypeCheckerOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"

#include <deque>
#include <queue>

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {



struct CompilerModuleBuffers {

  std::unique_ptr<llvm::MemoryBuffer> moduleBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer;
  std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer;

  // Constructor
  CompilerModuleBuffers(
      std::unique_ptr<llvm::MemoryBuffer> moduleBuffer,
      std::unique_ptr<llvm::MemoryBuffer> moduleDocBuffer = nullptr,
      std::unique_ptr<llvm::MemoryBuffer> moduleSourceInfoBuffer = nullptr)
      : moduleBuffer(std::move(moduleBuffer)),
        moduleDocBuffer(std::move(moduleDocBuffer)),
        moduleSourceInfoBuffer(std::move(moduleSourceInfoBuffer)) {}
};

using CompilerMemoryBuffers =
    llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>>;


class CompilerCommandLine;
class CompilerContext final {
  friend CompilerCommandLine;

private:
  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  LangOptions langOpts;

  stone::TargetOptions targetOpts;

  ModuleOptions moduleOpts;

  SyntaxOptions syntaxOpts;

  DiagnosticOptions diagOpts;

public:
  CompilerContext();

public:
  void SetTargetTriple(llvm::StringRef triple);
  void SetTargetTriple(const llvm::Triple &Triple);
  void SetWorkingDirectory();

public:
  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  stone::TargetOptions &GetTargetOptions() { return targetOpts; }
  const stone::TargetOptions &GetTargetOptions() const { return targetOpts; }

  SyntaxOptions &GetSyntaxOptions() { return syntaxOpts; }
  const SyntaxOptions &GetSyntaxOptions() const { return syntaxOpts; }

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

  ModuleOptions &GetModuleOptions() { return moduleOpts; }
  const ModuleOptions &GetModuleOptions() const { return moduleOpts; }
};

class CompilerCommandLine final {
public:
  CompilerCommandLine();

public:
  std::unique_ptr<CompilerContext> Parse(llvm::ArrayRef<const char *> args);

public:
  Status ParseCompilerAction(llvm::opt::InputArgList &args);
  Status ParseCompilerOptions(llvm::opt::InputArgList &args,
                              CompilerMemoryBuffers *buffers);
  Status ParseLangOptions(llvm::opt::InputArgList &args);
  Status ParseTypeCheckerOptions(llvm::opt::InputArgList &args);
  Status ParseSearchPathOptions(llvm::opt::InputArgList &args);
  Status ParseCodeGenOptions(llvm::opt::InputArgList &args);
  Status ParseTargetOptions(llvm::opt::InputArgList &args);
  llvm::StringRef ParseWorkDirectory(const llvm::opt::InputArgList &args);
};

} // namespace stone

#endif
