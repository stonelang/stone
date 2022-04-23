#ifndef STONE_BASIC_LANGOPTIONS_H
#define STONE_BASIC_LANGOPTIONS_H

#include <string>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {
class LangOptions final {
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

  /// The map of aliases and underlying names of imported or referenced modules.
  llvm::StringMap<llvm::StringRef> moduleAliasMap;

  /// The name of the module that the frontend is building.
  std::string moduleName;
  bool HasModuleName() { return moduleName.size() > 0; }

  /// The ABI name of the module that the compile is building, to be used in
  /// mangling and metadata.
  std::string moduleABIName;

  /// The name of the library to link against when using this module.
  std::string moduleLinkName;

public:
  LangOptions();

public:
  void SetTargetTriple(const llvm::Triple &triple);
  void SetTargetTriple(llvm::StringRef Triple);
};

} // namespace stone

#endif
