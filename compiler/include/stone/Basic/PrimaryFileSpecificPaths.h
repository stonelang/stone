#ifndef STONE_BASIC_PRIMARYSPECIFICPATHS_H
#define STONE_BASIC_PRIMARYSPECIFICPATHS_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SupplementaryOutputPaths.h"

namespace stone {
/// Holds all of the output paths, and debugging-info path that are
/// specific to which primary file is being compiled at the moment.

class PrimaryFileSpecificPaths final {
public:
  /// The name of the main output file,
  /// that is, the .o file for this input (or a file specified by -o).
  /// If there is no such file, contains an empty string. If the output
  /// is to be written to stdout, contains "-".
  String outputFilename;
  /// The name to report the main output file as being in the index store.
  /// This is equivalent to OutputFilename, unless -index-store-output-path
  /// was specified.
  String indexUnitOutputFilename;

  SupplementaryOutputPaths supplementaryOutputPaths;

  /// The name of the "main" input file, used by the debug info.
  String mainInputFilenameForDebugInfo;

  PrimaryFileSpecificPaths(
      StringRef outputFilename = llvm::StringRef(),
      StringRef indexUnitOutputFilename = llvm::StringRef(),
      StringRef mainInputFilenameForDebugInfo = llvm::StringRef(),
      SupplementaryOutputPaths supplementaryOutputPaths =
          SupplementaryOutputPaths())
      : outputFilename(outputFilename),
        indexUnitOutputFilename(indexUnitOutputFilename),
        supplementaryOutputPaths(supplementaryOutputPaths),
        mainInputFilenameForDebugInfo(mainInputFilenameForDebugInfo) {}

  bool HaveModuleOrModuleDocOutputPaths() const {
    return !supplementaryOutputPaths.moduleOutputPath.empty() ||
           !supplementaryOutputPaths.moduleDocOutputPath.empty();
  }
  bool HaveModuleSummaryOutputPath() const {
    return !supplementaryOutputPaths.moduleSummaryOutputPath.empty();
  }
};
} // namespace stone

#endif
