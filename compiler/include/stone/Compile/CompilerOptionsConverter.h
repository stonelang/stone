#ifndef STONE_COMPILE_LANGOPTIONSBUILDER_H
#define STONE_COMPILE_LANGOPTIONSBUILDER_H

#include "stone/Compile/CompilerOptions.h"
#include "stone/Context.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticListener.h"
#include "stone/Session/Options.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class CompilerOptionsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;
  LangOptions &langOpts;
  CompilerOptions &invocationOpts;

private:
  llvm::Optional<std::vector<std::string>>
      CachedOutputFilenamesFromCommandLineOrFileList;

  void HandleDebugCrashGroupArguments();
  void ComputeDebugTimeOptions();
  stone::Error ComputeFallbackModuleName();
  stone::Error ComputeModuleName();
  stone::Error ComputeModuleAliases();
  stone::Error ComputeMainAndSupplementaryOutputFilenames();
  void ComputeDumpScopeMapLocations();
  void ComputeHelpOptions();
  void ComputeImplicitImportModuleNames(llvm::opt::OptSpecifier id,
                                        bool isTestable);

  void ComputeLLVMArgs();
  void ComputePrintStatsOptions();
  void ComputeTBDOptions();

  stone::Error SetUpImmediateArgs();
  stone::Error CheckUnusedSupplementaryOutputPaths() const;
  stone::Error CheckForUnusedOutputPaths() const;

public:
  CompilerOptionsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args,
                           LangOptions &langOpts,
                           CompilerOptions &invocationOpts)
      : de(de), args(args), langOpts(langOpts), invocationOpts(invocationOpts) {
  }

public:
  stone::Error
  Convert(llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);
};
} // namespace stone
#endif
