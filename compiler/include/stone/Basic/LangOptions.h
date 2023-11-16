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

/// Kind of implicit platform conditions.
enum class PlatformConditionKind {
#define PLATFORM_CONDITION(LABEL, IDENTIFIER) LABEL,
#include "stone/Basic/PlatformConditionKind.def"
};

class LangOptions final {
public:

  // Targe related information 
  /// The name of the target triple to compile for.
  std::string triple;

  /// When compiling for the device side, contains the triple used to compile
  /// for the host.
  std::string hostTriple;

  /// If given, the name of the target CPU to generate code for.
  std::string cpu;

  /// If given, the name of the target CPU to tune code for.
  std::string tuneCPU;

  /// If given, the unit to use for floating point math.
  std::string fpMath;

  /// If given, the name of the target ABI to use.
  std::string abi;

  /// The EABI version to use
  llvm::EABI eABIVersion;

  /// If given, the version string of the linker in use.
  std::string linkerVersion;

  /// The list of target specific features to enable or disable, as written on
  /// the command line.
  std::vector<std::string> featuresAsWritten;

  /// The list of target specific features to enable or disable -- this should
  /// be a list of strings starting with by '+' or '-'.
  std::vector<std::string> features;

  /// The map of which features have been enabled disabled based on the command
  /// line.
  llvm::StringMap<bool> featureMap;

  /// If given, enables support for __int128_t and __uint128_t types.
  bool forceEnableInt128 = false;

  /// \brief If enabled, use 32-bit pointers for accessing const/local/shared
  /// address space.
  bool NVPTXUseShortPointers = false;

  /// \brief If enabled, allow AMDGPU unsafe floating point atomics.
  bool allowAMDGPUUnsafeFPAtomics = false;

  // The code model to be used as specified by the user. Corresponds to
  // CodeModel::Model enum defined in include/llvm/Support/CodeGen.h, plus
  // "default" for the case when the user has not explicitly specified a
  // code model.
  std::string codeModel;

  /// The version of the SDK which was used during the compilation.
  /// The option is used for two different purposes:
  /// * on darwin the version is propagated to LLVM where it's used
  ///   to support SDK Version metadata (See D55673).
  /// * CUDA compilation uses it to control parts of CUDA compilation
  ///   in clang that depend on specific version of the CUDA SDK.
  llvm::VersionTuple sdkVersion;

public:
  /// The target platform that we are running on.
  llvm::Triple Target;

  /// \brief The second target for a zippered build
  ///
  /// This represents the target and minimum deployment version for the
  /// second ('variant') target when performing a zippered build.
  /// For example, if the target is x86_64-apple-macosx10.14 then
  /// a target-variant of x86_64-apple-ios12.0-macabi will produce
  /// a zippered binary that can be loaded into both macCatalyst and
  /// macOS processes. A value of 'None' means no zippering will be
  /// performed.
  llvm::Optional<llvm::Triple> TargetVariant;

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
