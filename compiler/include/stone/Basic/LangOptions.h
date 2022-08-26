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

public:
  /// Access or distribution level of a library.
  enum class LibraryLevel : uint8_t {
    /// Application Programming Interface that is publicly distributed so
    /// public decls are really public and only @_spi decls are SPI.
    API,

    /// System Programming Interface that has restricted distribution
    /// all decls in the module are considered to be SPI including public ones.
    SPI,

    /// The library has some other undefined distribution.
    Other
  };
  LibraryLevel libraryLevel = LibraryLevel::API;

public:
  LangOptions();

public:
  void SetTargetTriple(const llvm::Triple &triple);
  void SetTargetTriple(llvm::StringRef Triple);
};

} // namespace stone

#endif
