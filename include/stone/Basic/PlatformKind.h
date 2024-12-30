#ifndef STONE_BASIC_PLATFORM_KIND_H
#define STONE_BASIC_PLATFORM_KIND_H

#include "stone/Basic/Basic.h"
#include "stone/Basic/LLVM.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {

class LangOptions;

/// Available platforms for the availability attribute.
enum class PlatformKind : uint8 {
  None,
#define AVAILABILITY_PLATFORM(X, PrettyName) X,
#include "stone/Basic/PlatformKind.def"
};

/// Returns the short string representing the platform, suitable for
/// use in availability specifications (e.g., "OSX").
StringRef PlatformString(PlatformKind platform);

/// Returns the platform kind corresponding to the passed-in short platform name
/// or None if such a platform kind does not exist.
std::optional<PlatformKind> PlatformFromString(StringRef Name);

/// Returns a human-readable version of the platform name as a string, suitable
/// for emission in diagnostics (e.g., "macOS").
StringRef PrettyPlatformString(PlatformKind platform);

/// Returns whether the passed-in platform is active, given the language
/// options. A platform is active if either it is the target platform or its
/// AppExtension variant is the target platform. For example, OS X is
/// considered active when the target operating system is OS X and app extension
/// restrictions are enabled, but OSXApplicationExtension is not considered
/// active when the target platform is OS X and app extension restrictions are
/// disabled. PlatformKind::None is always considered active.
/// If ForTargetVariant is true then for zippered builds the target-variant
/// triple will be used rather than the target to determine whether the
/// platform is active.
bool IsPlatformActive(PlatformKind Platform, const LangOptions &LangOpts,
                      bool ForTargetVariant = false);

/// Returns the target platform for the given language options.
PlatformKind TargetPlatform(const LangOptions &LangOpts);

/// Returns true when availability attributes from the "parent" platform
/// should also apply to the "child" platform for declarations without
/// an explicit attribute for the child.
bool InheritsAvailabilityFromPlatform(PlatformKind Child, PlatformKind Parent);

llvm::VersionTuple
CanonicalizePlatformVersion(PlatformKind platform,
                            const llvm::VersionTuple &version);

} // end namespace stone

#endif
