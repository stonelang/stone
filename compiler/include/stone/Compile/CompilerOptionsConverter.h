#ifndef STONE_COMPILE_COMPILEROPTIONSCONVERTER_H
#define STONE_COMPILE_COMPILEROPTIONSCONVERTER_H

#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Diag/DiagnosticConsumer.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

#include "llvm/Option/ArgList.h"

namespace stone {

class CompilerOptionsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;
  LangOptions &langOpts;
  CompilerOptions &compilerOpts;

private:
  llvm::Optional<std::vector<std::string>>
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

public:
  CompilerOptionsConverter(const llvm::opt::ArgList &args, DiagnosticEngine &de,
                           LangOptions &langOpts,
                           CompilerOptions &compilerOpts);

public:
  Status
  Convert(llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);
};
} // namespace stone
#endif
