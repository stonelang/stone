#ifndef STONE_COMPILE_COMPILEROPTIONSCONVERTER_H
#define STONE_COMPILE_COMPILEROPTIONSCONVERTER_H

#include "stone/AST/Diagnostics.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Support/Options.h"

namespace stone {

class CompilerOptionsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;
  LangOptions &langOpts;
  CompilerOptions &compilerOpts;

private:
  std::optional<std::vector<std::string>>
      CachedOutputFilenamesFromCommandLineOrFileList;

  void HandleDebugCrashGroupArguments();
  void ComputeDebugTimeOptions();
  Status ComputeFallbackModuleName();
  Status ComputeModuleName();
  Status ComputeModuleAliases();
  Status ComputeMainAndSupplementaryOutputFilenames();
  void ComputeDumpScopeMapLocations();
  void ComputeHelpOptions();
  void ComputeImplicitImportModuleNames(llvm::opt::OptSpecifier id,
                                        bool isTestable);

  void ComputeLLVMArgs();
  void ComputePrintStatsOptions();
  void ComputeTBDOptions();

  Status SetUpImmediateArgs();
  Status CheckUnusedSupplementaryOutputPaths() const;
  Status CheckForUnusedOutputPaths() const;

private:
  static CompilerActionKind ComputeActionKind(const unsigned modeOptID);

public:
  CompilerOptionsConverter(const llvm::opt::ArgList &args, DiagnosticEngine &de,
                           LangOptions &langOpts,
                           CompilerOptions &compilerOpts);

public:
  Status
  Convert(llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);
  static CompilerActionKind ComputeActionKind(const llvm::opt::ArgList &args);
};
} // namespace stone
#endif
