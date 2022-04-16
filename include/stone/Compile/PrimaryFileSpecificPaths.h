#ifndef STONE_COMPILE_PRIMARYSPECIFICPATHS_H
#define STONE_COMPILE_PRIMARYSPECIFICPATHS_H

#include "stone/Basic/LLVM.h"
#include "stone/Compile/AdditionalOutputPaths.h"
#include "llvm/ADT/StringRef.h"

#include <string>

namespace stone {
/// Holds all of the output paths, and debugging-info path that are
/// specific to which primary file is being compiled at the moment.

class PrimaryFileSpecificPaths {
public:
  /// The name of the main output file,
  /// that is, the .o file for this input (or a file specified by -o).
  /// If there is no such file, contains an empty string. If the output
  /// is to be written to stdout, contains "-".
  std::string outputFilename;

  /// The name to report the main output file as being in the index store.
  /// This is equivalent to OutputFilename, unless -index-store-output-path
  /// was specified.
  std::string indexUnitOutputFilename;

  AdditionalOutputPaths additionalOutputPaths;

  /// The name of the "main" input file, used by the debug info.
  std::string mainInputFilenameForDebugInfo;

  PrimaryFileSpecificPaths(
      StringRef outputFilename = llvm::StringRef(),
      StringRef indexUnitOutputFilename = llvm::StringRef(),
      StringRef mainInputFilenameForDebugInfo = llvm::StringRef(),
      AdditionalOutputPaths additionalOutputPaths = AdditionalOutputPaths())
      : outputFilename(outputFilename),
        indexUnitOutputFilename(indexUnitOutputFilename),
        additionalOutputPaths(additionalOutputPaths),
        mainInputFilenameForDebugInfo(mainInputFilenameForDebugInfo) {}

  bool HaveModuleOrModuleDocOutputPaths() const {
    return !additionalOutputPaths.moduleOutputPath.empty() ||
           !additionalOutputPaths.moduleDocOutputPath.empty();
  }
  bool HaveModuleSummaryOutputPath() const {
    return !additionalOutputPaths.moduleSummaryOutputPath.empty();
  }
};
} // namespace stone

#endif
