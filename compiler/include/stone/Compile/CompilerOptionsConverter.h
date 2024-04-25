#ifndef STONE_COMPILE_COMPILEROPTIONSCONVERTER_H
#define STONE_COMPILE_COMPILEROPTIONSCONVERTER_H

#include "stone/Basic/Status.h"
#include "stone/Compile/CompilerOptions.h"
#include "stone/Support/DiagnosticConsumer.h"
#include "stone/Support/Options.h"
#include "stone/Syntax/Diagnostics.h"

namespace stone {

class CompilerArgList final {
  const llvm::opt::ArgList &args;

public:
  CompilerArgList(const llvm::opt::ArgList &args);

public:
  const llvm::opt::ArgList &GetArgs() const;
};

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
  static CompilerAction ComputeCompilerAction(const unsigned modeOptID);

public:
  CompilerOptionsConverter(const llvm::opt::ArgList &args, DiagnosticEngine &de,
                           LangOptions &langOpts,
                           CompilerOptions &compilerOpts);

public:
  Status
  Convert(llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);

  static CompilerAction ComputeCompilerAction(const llvm::opt::ArgList &args);
};
} // namespace stone
#endif
