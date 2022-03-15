#ifndef STONE_CORE_SYSTEMOPTIONS_H
#define STONE_CORE_SYSTEMOPTIONS_H

#include <string>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {

class SystemOptions final {
public:
  /// The target platform that we are running on.
  llvm::Triple target;

  /// This represents the statistics generated
  bool printStatistics = true;

  /// This represents the diatnostics generated
  bool printDiagnostics = true;
  ///
  bool showTimer = false;
  ///
  bool useMalloc = false;

  /// The name of the module being built.
  std::string moduleName;
  bool HasModuleName() { return moduleName.size() > 0; }

  /// The path to the SDK against which to build.
  /// (If empty, this implies no SDK.)
  std::string sdkPath;
  bool HasSDKPath() { return sdkPath.size() > 0; }

public:
  SystemOptions();

public:
  void SetTargetTriple(const llvm::Triple &triple);
  void SetTargetTriple(llvm::StringRef Triple);
};

} // namespace stone

#endif
