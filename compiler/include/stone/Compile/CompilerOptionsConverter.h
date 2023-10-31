#ifndef STONE_COMPILE_LANGOPTIONSBUILDER_H
#define STONE_COMPILE_LANGOPTIONSBUILDER_H

#include "stone/Compile/CompilerOptions.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticListener.h"
#include "stone/Lang.h"
#include "stone/Options/Options.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class CompilerOptionsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;
  LangOptions &langOpts;
  CompilerOptions &compilerOpts;
  ModuleOptions &moduleOpts;

private:
  llvm::Optional<std::vector<std::string>>
      CachedOutputFilenamesFromCommandLineOrFileList;

  void HandleDebugCrashGroupArguments();
  void ComputeDebugTimeOptions();
  Error ComputeFallbackModuleName();
  Error ComputeModuleName();
  Error ComputeModuleAliases();
  Error ComputeMainAndSupplementaryOutputFilenames();
  void ComputeDumpScopeMapLocations();
  void ComputeHelpOptions();
  void ComputeImplicitImportModuleNames(llvm::opt::OptSpecifier id,
                                        bool isTestable);

  void ComputeLLVMArgs();
  void ComputePrintStatsOptions();
  void ComputeTBDOptions();

  Error SetUpImmediateArgs();
  Error CheckUnusedSupplementaryOutputPaths() const;
  Error CheckForUnusedOutputPaths() const;

public:
  CompilerOptionsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args,
                           LangOptions &langOpts, CompilerOptions &compilerOpts,
                           ModuleOptions &moduleOpts);

public:
  Error
  Convert(llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);
};
} // namespace stone
#endif
