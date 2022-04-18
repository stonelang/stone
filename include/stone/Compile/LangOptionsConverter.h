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
  bool ComputeFallbackModuleName();
  bool ComputeModuleName();
  bool ComputeModuleAliases();
  bool ComputeMainAndSupplementaryOutputFilenames();
  void ComputeDumpScopeMapLocations();
  void ComputeHelpOptions();
  void ComputeImplicitImportModuleNames(llvm::opt::OptSpecifier id,
                                        bool isTestable);

  void ComputeLLVMArgs();
  void ComputePrintStatsOptions();
  void ComputeTBDOptions();

  bool SetUpImmediateArgs();
  bool CheckUnusedSupplementaryOutputPaths() const;
  bool CheckForUnusedOutputPaths() const;

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
