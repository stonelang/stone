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
#include "llvm/Support/Regex.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"

namespace stone {

/// Kind of implicit platform conditions.
enum class PlatformConditionKind {
#define PLATFORM_CONDITION(LABEL, IDENTIFIER) LABEL,
#include "stone/Basic/PlatformConditionKind.def"
};

class LangOptions final {

public:
  /// The target platform that we are running on.
  llvm::Triple DefaultTargetTriple;

  /// \brief The second target for a zippered build
  ///
  /// This represents the target and minimum deployment version for the
  /// second ('variant') target when performing a zippered build.
  /// For example, if the target is x86_64-apple-macosx10.14 then
  /// a target-variant of x86_64-apple-ios12.0-macabi will produce
  /// a zippered binary that can be loaded into both macCatalyst and
  /// macOS processes. A value of 'None' means no zippering will be
  /// performed.
  std::optional<llvm::Triple> TargetVariant;

  /// This represents the statistics generated
  bool printStatistics = true;

  /// This represents the diatnostics generated
  bool printDiagnostics = true;
  ///
  bool showTimer = false;
  ///
  bool useMalloc = false;

  /// Enable 'availability' restrictions for App Extensions.
  bool EnableAppExtensionRestrictions = false;

public:
  enum class ThreadModelKind {
    /// POSIX Threads.
    POSIX,
    /// Single Threaded Environment.
    Single
  };

  ///
  ThreadModelKind threadModelKind = ThreadModelKind::POSIX;

private:
  llvm::SmallVector<std::pair<PlatformConditionKind, std::string>, 6>
      PlatformConditionValues;

  llvm::SmallVector<std::string, 2> CustomConditionalCompilationFlags;

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
  /// Sets an implicit platform condition.
  void AddPlatformConditionValue(PlatformConditionKind Kind,
                                 llvm::StringRef Value) {
    assert(!Value.empty());
    PlatformConditionValues.emplace_back(Kind, Value.str());
  }

  /// Removes all values added with addPlatformConditionValue.
  void ClearAllPlatformConditionValues() { PlatformConditionValues.clear(); }

  /// Returns the value for the given platform condition or an empty string.
  llvm::StringRef GetPlatformConditionValue(PlatformConditionKind Kind) const;

  /// Check whether the given platform condition matches the given value.
  bool CheckPlatformCondition(PlatformConditionKind Kind,
                              llvm::StringRef Value) const;

public:
  class TargetResult final {
    friend LangOptions;

  private:
    bool UnsupportedOS;
    bool UnsupportedArch;
    TargetResult() : UnsupportedOS(false), UnsupportedArch(false) {}

  public:
    bool IsUnsupported() { return UnsupportedOS || UnsupportedArch; }
  };
  TargetResult SetTarget(llvm::Triple triple);
  TargetResult SetTarget(llvm::StringRef triple);
};

} // namespace stone

#endif
