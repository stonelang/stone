#ifndef STONE_BASIC_MODULEOPTIONS_H
#define STONE_BASIC_MODULEOPTIONS_H

#include <string>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Triple.h"

namespace stone {

enum class ModuleOutputMode : uint8_t {
  None = 0,
  Single,
  Whole,
};

class ModuleOptions final {
public:
  /// The map of aliases and underlying names of imported or referenced modules.
  llvm::StringMap<llvm::StringRef> moduleAliasMap;

  /// The name of the module that the invocation is building.
  std::string moduleName;
  bool HasModuleName() const { return !moduleName.empty(); }

  /// The ABI name of the module that the compile is building, to be used in
  /// mangling and metadata.
  std::string moduleABIName;

  /// The name of the library to link against when using this module.
  std::string moduleLinkName;

  ModuleOutputMode moduleOutputMode = ModuleOutputMode::None;
};

} // namespace stone

#endif
