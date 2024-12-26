#ifndef STONE_COMPILE_COMPILERINVOCATION_H
#define STONE_COMPILE_COMPILERINVOCATION_H

#include "stone/AST/Diagnostics.h"
#include "stone/AST/TypeCheckerOptions.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Support/DiagnosticOptions.h"
#include "stone/Support/Options.h"

#include "stone/AST/ClangImporter.h"
#include "stone/AST/Module.h"
#include "stone/AST/SearchPath.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"
#include "stone/Basic/Status.h"
#include "stone/Basic/TargetOptions.h"

#include "clang/Basic/FileManager.h"
#include "clang/Basic/FileSystemOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Option/ArgList.h"

#include <memory>

namespace stone {

class CompilerDiagnosticFormatter final : public DiagnosticFormatter {
  SrcMgr &SM;

public:
  CompilerDiagnosticFormatter(SrcMgr &SM);

public:
  SrcMgr &GetSrcMgr() { return SM; }

  void FormatDiagnosticText(
      llvm::raw_ostream &Out, StringRef InText,
      ArrayRef<DiagnosticArgument> FormatArgs,
      DiagnosticFormatOptions FormatOpts = DiagnosticFormatOptions()) override;
};

class CompilerDiagnosticPrinter final : public TextDiagnosticPrinter {

  std::unique_ptr<CompilerDiagnosticFormatter> formatter;

public:
  CompilerDiagnosticPrinter(SrcMgr &SM);

public:
  DiagnosticFormatter *GetDiagnosticFormatter() override;
};

using ConfigurationFileBuffers =
    llvm::SmallVector<std::unique_ptr<llvm::MemoryBuffer>, 4>;

struct ModuleBuffers final {
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

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

  CompilerOptions compilerOpts;

  /// Options for generating code
  CodeGenOptions codeGenOpts;

  /// The options for searching libs
  SearchPathOptions searchPathOpts;

  /// The options for type-checking
  TypeCheckerOptions typeCheckerOpts;

  LangOptions langOpts;

  DiagnosticOptions diagOpts;

  clang::FileSystemOptions fileSystemOpts;

  TargetOptions targetOptions;

  std::unique_ptr<ClangImporter> clangImporter;

  /// The parsed input arg list
  std::unique_ptr<llvm::opt::InputArgList> inputArgList;

public:
  CompilerInvocation();

public:
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.addConsumer(consumer);
  }
  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.removeConsumer(consumer);
  }

  DiagnosticEngine &GetDiags() { return diags; }
  const DiagnosticEngine &GetDiags() const { return diags; }

  bool HasError() { return diags.hadAnyError(); }
  SrcMgr &GetSrcMgr() { return srcMgr; }

public:
  const llvm::opt::OptTable &GetOptTable() const {
    return GetCompilerOptions().GetOptTable();
  }
  llvm::opt::InputArgList &GetInputArgList() { return *inputArgList; }

  CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  const CompilerOptions &GetCompilerOptions() const { return compilerOpts; }

  CodeGenOptions &GetCodeGenOptions() { return codeGenOpts; }
  const CodeGenOptions &GetCodeGenOptions() const { return codeGenOpts; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  stone::TargetOptions &GetTargetOptions() { return targetOptions; }
  const stone::TargetOptions &GetTargetContext() const { return targetOptions; }

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

  void SetTargetTriple(llvm::StringRef triple);

  /// Set the main exec path
  void SetMainExecutablePath(llvm::StringRef executablePath) {
    compilerOpts.mainExecutablePath = executablePath;
  }

  /// Set the main exec path
  void SetMainExecutableName(llvm::StringRef executableName) {
    compilerOpts.mainExecutableName = executableName;
  }

public:
  Status SetupClang(llvm::ArrayRef<const char *> args, const char *arg0);
  ClangImporter &GetClangImporter() { return *clangImporter; }

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
  Status ParseArgs(llvm::ArrayRef<const char *> args);
};

} // namespace stone
#endif