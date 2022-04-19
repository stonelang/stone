#ifndef STONE_COMPILE_LANGOPTIONSBUILDER_H
#define STONE_COMPILE_LANGOPTIONSBUILDER_H

#include "stone/Basic/Context.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/DiagnosticListener.h"
#include "stone/Compile/LangOptions.h"
#include "stone/Session/Options.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class LangOptionsConverter {
  DiagnosticEngine &de;
  const llvm::opt::ArgList &args;
  const Mode &mode;
  LangOptions &langOpts;

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
  LangOptionsConverter(DiagnosticEngine &de, const llvm::opt::ArgList &args,
                       const Mode &mode, LangOptions &langOpts)
      : de(de), args(args), mode(mode), langOpts(langOpts) {}

public:
  stone::Error
  Convert(llvm::SmallVectorImpl<std::unique_ptr<llvm::MemoryBuffer>> *buffers);
};
} // namespace stone
#endif
