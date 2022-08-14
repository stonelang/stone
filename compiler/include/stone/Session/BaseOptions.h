#ifndef STONE_SESSION_BASEOPTIONS_H
#define STONE_SESSION_BASEOPTIONS_H

#include "stone/Basic/File.h"
#include "stone/Session/Mode.h"
#include "stone/Session/Options.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/ArgList.h"

namespace stone {

class BaseOptions {
  std::unique_ptr<Mode> mode;

  llvm::SmallString<128> currentPath;

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

  llvm::StringRef GetCurrentPath() { return currentPath.data(); }

  Mode &GetMode() { return *mode; }
  const Mode &GetMode() const { return *mode; }

public:
  BaseOptions(std::unique_ptr<Mode> mode);

  // public:
  //   void SetTargetTriple(const llvm::Triple &triple);
  //   void SetTargetTriple(llvm::StringRef Triple);
};
} // namespace stone
#endif