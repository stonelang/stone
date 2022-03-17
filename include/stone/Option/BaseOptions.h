#ifndef STONE_OPTION_BASEOPTIONS_H
#define STONE_OPTION_BASEOPTIONS_H

#include "stone/Core/File.h"
#include "stone/Option/Mode.h"
#include "stone/Option/Options.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Option/ArgList.h"

namespace stone {
namespace opts {

class BaseOptions {

  // /// The target platform that we are running on.
  // llvm::Triple target;

  // /// This represents the statistics generated
  // bool printStatistics = true;

  // /// This represents the diatnostics generated
  // bool printDiagnostics = true;
  // ///
  // bool showTimer = false;
  // ///
  // bool useMalloc = false;

  // /// The name of the module being built.
  // std::string moduleName;
  // bool HasModuleName() { return moduleName.size() > 0; }

  // /// The path to the SDK against which to build.
  // /// (If empty, this implies no SDK.)
  // std::string sdkPath;
  // bool HasSDKPath() { return sdkPath.size() > 0; }

public:
  /// a SmallVector of ctx files
  file::Files inputFiles;

  /// The file input type
  file::Type inputFileType = file::Type::None;

  /// The output file type which should be used for the sc
  file::Type outputFileType = file::Type::None;

  ModeKind defaultModeKind = ModeKind::None;

public:
  BaseOptions();

  // public:
  //   void SetTargetTriple(const llvm::Triple &triple);
  //   void SetTargetTriple(llvm::StringRef Triple);
};
} // namespace opts
} // namespace stone
#endif