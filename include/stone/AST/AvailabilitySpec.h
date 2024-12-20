#ifndef STONE_AST_AVAILABILITYSPEC_H
#define STONE_AST_AVAILABILITYSPEC_H

#include "stone/AST/Identifier.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/PlatformKind.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/Support/VersionTuple.h"

namespace stone {

class ASTContext;

enum class VersionComparison { None = 0, GreaterThanEqual };

enum class AvailabilitySpecKind {
  /// A platform-version constraint of the form "PlatformName X.Y.Z"
  PlatformVersionConstraint,

  /// A wildcard constraint, spelled '*', that is equivalent
  /// to CurrentPlatformName >= MinimumDeploymentTargetVersion
  OtherPlatform,

  /// A language-version constraint of the form "stone X.Y.Z"
  LanguageVersionConstraint,

  /// A PackageDescription version constraint of the form "_PackageDescription
  /// X.Y.Z"
  PackageDescriptionVersionConstraint,
};

/// The root class for specifications of API availability in availability
/// queries.
class AvailabilitySpec : public MemoryAllocation<AvailabilitySpec> {
  AvailabilitySpecKind Kind;

public:
  AvailabilitySpec(AvailabilitySpecKind Kind) : Kind(Kind) {}

  AvailabilitySpecKind getKind() const { return Kind; }

  SrcRange getSrcRange() const;
};

/// An availability specification that guards execution based on the
/// run-time platform and version, e.g., OS X >= 10.10.
class PlatformVersionConstraintAvailabilitySpec : public AvailabilitySpec {
  PlatformKind Platform;
  SrcLoc PlatformLoc;

  llvm::VersionTuple Version;

  // For macOS Big Sur, we canonicalize 10.16 to 11.0 for compile-time
  // checking since clang canonicalizes availability markup. However, to
  // support Beta versions of macOS Big Sur where the OS
  // reports 10.16 at run time, we need to compare against 10.16,
  //
  // This means for:
  //
  // if #available(macOS 10.16, *) { ... }
  //
  // we need to keep around both a canonical version for use in compile-time
  // checks and an uncanonicalized version for the version to actually codegen
  // with.
  llvm::VersionTuple RuntimeVersion;

  SrcRange VersionSrcRange;

  // Location of the macro expanded to create this spec.
  SrcLoc MacroLoc;

public:
  PlatformVersionConstraintAvailabilitySpec(PlatformKind Platform,
                                            SrcLoc PlatformLoc,
                                            llvm::VersionTuple Version,
                                            llvm::VersionTuple RuntimeVersion,
                                            SrcRange VersionSrcRange)
      : AvailabilitySpec(AvailabilitySpecKind::PlatformVersionConstraint),
        Platform(Platform), PlatformLoc(PlatformLoc), Version(Version),
        RuntimeVersion(RuntimeVersion), VersionSrcRange(VersionSrcRange) {}

  /// The required platform.
  PlatformKind getPlatform() const { return Platform; }
  SrcLoc getPlatformLoc() const { return PlatformLoc; }

  /// Returns true when the constraint is for a platform that was not
  /// recognized. This enables better recovery during parsing but should never
  /// be true after parsing is completed.
  bool isUnrecognizedPlatform() const { return Platform == PlatformKind::None; }

  // The platform version to compare against.
  llvm::VersionTuple getVersion() const { return Version; }
  SrcRange getVersionSrcRange() const { return VersionSrcRange; }

  // The version to be used in codegen for version comparisons at run time.
  // This is required to support beta versions of macOS Big Sur that
  // report 10.16 at run time.
  llvm::VersionTuple getRuntimeVersion() const { return RuntimeVersion; }

  SrcRange getSrcRange() const;

  // Location of the macro expanded to create this spec.
  SrcLoc getMacroLoc() const { return MacroLoc; }
  void setMacroLoc(SrcLoc loc) { MacroLoc = loc; }

  void print(raw_ostream &OS, unsigned Indent) const;

  static bool classof(const AvailabilitySpec *Spec) {
    return Spec->getKind() == AvailabilitySpecKind::PlatformVersionConstraint;
  }

  // void *operator new(
  //     size_t Bytes, ASTContext &C,
  //     unsigned Alignment =
  //     alignof(PlatformVersionConstraintAvailabilitySpec)) {
  //   return AvailabilitySpec::operator new(Bytes, C,
  //   MemoryAllocationArena::Stoneanent,
  //                                         Alignment);
  // }
};

/// An availability specification that guards execution based on the
/// compile-time platform agnostic version, e.g., stone >= 3.0.1,
/// package-description >= 4.0.
class PlatformAgnosticVersionConstraintAvailabilitySpec
    : public AvailabilitySpec {
  SrcLoc PlatformAgnosticNameLoc;

  llvm::VersionTuple Version;
  SrcRange VersionSrcRange;

public:
  PlatformAgnosticVersionConstraintAvailabilitySpec(
      AvailabilitySpecKind AvailabilitySpecKind, SrcLoc PlatformAgnosticNameLoc,
      llvm::VersionTuple Version, SrcRange VersionSrcRange)
      : AvailabilitySpec(AvailabilitySpecKind),
        PlatformAgnosticNameLoc(PlatformAgnosticNameLoc), Version(Version),
        VersionSrcRange(VersionSrcRange) {
    assert(AvailabilitySpecKind ==
               AvailabilitySpecKind::LanguageVersionConstraint ||
           AvailabilitySpecKind ==
               AvailabilitySpecKind::PackageDescriptionVersionConstraint);
  }

  SrcLoc getPlatformAgnosticNameLoc() const { return PlatformAgnosticNameLoc; }

  // The platform version to compare against.
  llvm::VersionTuple getVersion() const { return Version; }
  SrcRange getVersionSrcRange() const { return VersionSrcRange; }

  SrcRange getSrcRange() const;

  bool isLanguageVersionSpecific() const {
    return getKind() == AvailabilitySpecKind::LanguageVersionConstraint;
  }

  void print(raw_ostream &OS, unsigned Indent) const;

  static bool classof(const AvailabilitySpec *Spec) {
    return Spec->getKind() == AvailabilitySpecKind::LanguageVersionConstraint ||
           Spec->getKind() ==
               AvailabilitySpecKind::PackageDescriptionVersionConstraint;
  }

  // void *operator new(size_t Bytes, ASTContext &C,
  //                    unsigned Alignment = alignof(
  //                        PlatformAgnosticVersionConstraintAvailabilitySpec))
  //                        {
  //   return AvailabilitySpec::operator new(Bytes, C,
  //   MemoryAllocationArena::Stoneanent,
  //                                         Alignment);
  // }
};

/// A wildcard availability specification that guards execution
/// by checking that the run-time version is greater than the minimum
/// deployment target. This specification is designed to ease porting
/// to new platforms. Because new platforms typically branch from
/// existing platforms, the wildcard allows an #available() check to do the
/// "right" thing (executing the guarded branch) on the new platform without
/// requiring a modification to every availability guard in the program. Note
/// that we still do compile-time availability checking with '*', so the
/// compiler will still catch references to potentially unavailable symbols.
class OtherPlatformAvailabilitySpec : public AvailabilitySpec {
  SrcLoc StarLoc;

public:
  OtherPlatformAvailabilitySpec(SrcLoc StarLoc)
      : AvailabilitySpec(AvailabilitySpecKind::OtherPlatform),
        StarLoc(StarLoc) {}

  SrcLoc getStarLoc() const { return StarLoc; }

  SrcRange getSrcRange() const { return SrcRange(StarLoc, StarLoc); }

  void print(raw_ostream &OS, unsigned Indent) const;

  static bool classof(const AvailabilitySpec *Spec) {
    return Spec->getKind() == AvailabilitySpecKind::OtherPlatform;
  }

  // void *
  // operator new(size_t Bytes, ASTContext &C,
  //              unsigned Alignment = alignof(OtherPlatformAvailabilitySpec)) {
  //   return AvailabilitySpec::operator new(Bytes, C,
  //   AllocationArena::Stoneanent,
  //                                         Alignment);
  // }
};
} // namespace stone
#endif