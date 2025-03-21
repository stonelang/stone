#ifndef STONE_AST_AVAILABILITY_H
#define STONE_AST_AVAILABILITY_H

#include "stone/AST/Attribute.h"
#include "stone/AST/Type.h"
#include "stone/Basic/LLVM.h"
#include "llvm/Support/VersionTuple.h"

#include <optional>

namespace stone {

class ASTContext;
class Decl;

/// A lattice of version ranges of the form [x.y.z, +Inf).
class VersionRange {
  // The lattice ordering is linear:
  // Empty <= ... <= [10.10.0,+Inf) <= ... [10.1.0,+Inf) <=  ... <= All
  // and corresponds to set inclusion.

  // The concretization of lattice elements is:
  //    Empty: empty
  //    All: all versions
  //    x.y.x: all versions greater than or equal to x.y.z

  enum class ExtremalRange { Empty, All };

  // A version range is either an extremal value (Empty, All) or
  // a single version tuple value representing the lower end point x.y.z of a
  // range [x.y.z, +Inf).
  union {
    llvm::VersionTuple LowerEndpoint;
    ExtremalRange ExtremalValue;
  };

  unsigned HasLowerEndpoint : 1;

public:
  /// Returns true if the range of versions is empty, or false otherwise.
  bool isEmpty() const {
    return !HasLowerEndpoint && ExtremalValue == ExtremalRange::Empty;
  }

  /// Returns true if the range includes all versions, or false otherwise.
  bool isAll() const {
    return !HasLowerEndpoint && ExtremalValue == ExtremalRange::All;
  }

  /// Returns true if the range has a lower end point; that is, if it is of
  /// the form [X, +Inf).
  bool hasLowerEndpoint() const { return HasLowerEndpoint; }

  /// Returns the range's lower endpoint.
  const llvm::VersionTuple &getLowerEndpoint() const {
    assert(HasLowerEndpoint);
    return LowerEndpoint;
  }

  /// Returns a representation of this range as a string for debugging purposes.
  std::string getAsString() const {
    if (isEmpty()) {
      return "empty";
    } else if (isAll()) {
      return "all";
    } else {
      return "[" + getLowerEndpoint().getAsString() + ",+Inf)";
    }
  }

  /// Returns true if all versions in this range are also in the Other range.
  bool isContainedIn(const VersionRange &Other) const {
    if (isEmpty() || Other.isAll())
      return true;

    if (isAll() || Other.isEmpty())
      return false;

    // [v1, +Inf) is contained in [v2, +Inf) if v1 >= v2
    return getLowerEndpoint() >= Other.getLowerEndpoint();
  }

  // Returns true if all the versions in the Other range are versions in this
  // range and the ranges are not equal.
  bool isSupersetOf(const VersionRange &Other) const {
    if (isEmpty() || Other.isAll())
      return false;

    if (isAll() || Other.isEmpty())
      return true;

    return getLowerEndpoint() < Other.getLowerEndpoint();
  }

  /// Mutates this range to be a best-effort underapproximation of
  /// the intersection of itself and Other. This is the
  /// meet operation (greatest lower bound) in the version range lattice.
  void intersectWith(const VersionRange &Other) {
    // With the existing lattice this operation is precise. If the lattice
    // is ever extended it is important that this operation be an
    // underapproximation of intersection.

    if (isEmpty() || Other.isAll())
      return;

    if (isAll() || Other.isEmpty()) {
      *this = Other;
      return;
    }

    // The g.l.b of [v1, +Inf), [v2, +Inf) is [max(v1,v2), +Inf)
    const llvm::VersionTuple maxVersion =
        std::max(this->getLowerEndpoint(), Other.getLowerEndpoint());

    setLowerEndpoint(maxVersion);
  }

  /// Mutates this range to be the union of itself and Other. This is the
  /// join operator (least upper bound) in the version range lattice.
  void unionWith(const VersionRange &Other) {
    // With the existing lattice this operation is precise. If the lattice
    // is ever extended it is important that this operation be an
    // overapproximation of union.
    if (isAll() || Other.isEmpty())
      return;

    if (isEmpty() || Other.isAll()) {
      *this = Other;
      return;
    }

    // The l.u.b of [v1, +Inf), [v2, +Inf) is [min(v1,v2), +Inf)
    const llvm::VersionTuple minVersion =
        std::min(this->getLowerEndpoint(), Other.getLowerEndpoint());

    setLowerEndpoint(minVersion);
  }

  /// Mutates this range to be a best effort over-approximation of the
  /// intersection of the concretizations of this version range and Other.
  void constrainWith(const VersionRange &Other) {
    // We can use intersection for this because the lattice is multiplicative
    // with respect to concretization--that is, the concretization
    // of Range1 meet Range2 is equal to the intersection of the
    // concretization of Range1 and the concretization of Range2.
    // This will change if we add (-Inf, v) to our version range lattice.
    intersectWith(Other);
  }

  /// Returns a version range representing all versions.
  static VersionRange all() { return VersionRange(ExtremalRange::All); }

  /// Returns a version range representing no versions.
  static VersionRange empty() { return VersionRange(ExtremalRange::Empty); }

  /// Returns a version range representing all versions greater than or equal
  /// to the passed-in version.
  static VersionRange allGTE(const llvm::VersionTuple &EndPoint) {
    return VersionRange(EndPoint);
  }

private:
  VersionRange(const llvm::VersionTuple &LowerEndpoint) {
    setLowerEndpoint(LowerEndpoint);
  }

  VersionRange(ExtremalRange ExtremalValue) { setExtremalRange(ExtremalValue); }

  void setExtremalRange(ExtremalRange Version) {
    HasLowerEndpoint = 0;
    ExtremalValue = Version;
  }

  void setLowerEndpoint(const llvm::VersionTuple &Version) {
    HasLowerEndpoint = 1;
    LowerEndpoint = Version;
  }
};

/// Records the reason a declaration is potentially unavailable.
class UnavailabilityReason {
private:
  VersionRange RequiredDeploymentRange;

  explicit UnavailabilityReason(const VersionRange RequiredDeploymentRange)
      : RequiredDeploymentRange(RequiredDeploymentRange) {}

public:
  static UnavailabilityReason requiresVersionRange(const VersionRange Range) {
    return UnavailabilityReason(Range);
  }

  const VersionRange &getRequiredOSVersionRange() const {
    return RequiredDeploymentRange;
  }

  /// Returns true if the required OS version range's lower endpoint is at or
  /// below the deployment target of the given ASTContext.
  bool requiresDeploymentTargetOrEarlier(ASTContext &Ctx) const;
};

/// Represents everything that a particular chunk of code may assume about its
/// runtime environment.
///
/// The AvailabilityContext structure forms a [lattice][], which allows it to
/// have meaningful union and intersection operations ("join" and "meet"),
/// which use conservative approximations to prevent availability violations.
/// See #unionWith, #intersectWith, and #constrainWith.
///
/// [lattice]: http://mathworld.wolfram.com/Lattice.html
class AvailabilityContext {
  VersionRange OSVersion;
  std::optional<bool> SPI;

public:
  /// Creates a context that requires certain versions of the target OS.
  explicit AvailabilityContext(VersionRange OSVersion,
                               std::optional<bool> SPI = std::nullopt)
      : OSVersion(OSVersion), SPI(SPI) {}

  /// Creates a context that imposes the constraints of the ASTContext's
  /// deployment target.
  static AvailabilityContext forDeploymentTarget(ASTContext &Ctx);

  /// Creates a context that imposes the constraints of the ASTContext's
  /// inlining target (i.e. minimum inlining version).
  static AvailabilityContext forInliningTarget(ASTContext &Ctx);

  /// Creates a context that imposes no constraints.
  ///
  /// \see isAlwaysAvailable
  static AvailabilityContext alwaysAvailable() {
    return AvailabilityContext(VersionRange::all());
  }

  /// Creates a context that can never actually occur.
  ///
  /// \see isKnownUnreachable
  static AvailabilityContext neverAvailable() {
    return AvailabilityContext(VersionRange::empty());
  }

  /// Returns the range of possible OS versions required by this context.
  VersionRange getOSVersion() const { return OSVersion; }

  /// Returns true if \p other makes stronger guarantees than this context.
  ///
  /// That is, `a.isContainedIn(b)` implies `a.union(b) == b`.
  bool isContainedIn(const AvailabilityContext &other) const {
    return OSVersion.isContainedIn(other.OSVersion);
  }

  /// Returns true if \p other is a strict subset of this context.
  ///
  /// That is, `a.isSupersetOf(b)` implies `a != b` and `a.union(b) == a`.
  bool isSupersetOf(const AvailabilityContext &other) const {
    return OSVersion.isSupersetOf(other.OSVersion);
  }

  /// Returns true if this context has constraints that make it impossible to
  /// actually occur.
  ///
  /// For example, the else branch of a `#available` check for iOS 8.0 when the
  /// containing function already requires iOS 9.
  bool isKnownUnreachable() const { return OSVersion.isEmpty(); }

  /// Returns true if there are no constraints on this context; that is,
  /// nothing can be assumed.
  bool isAlwaysAvailable() const { return OSVersion.isAll(); }

  /// Produces an under-approximation of the intersection of the two
  /// availability contexts.
  ///
  /// That is, if the intersection can't be represented exactly, prefer
  /// treating some valid deployment environments as unavailable. This is the
  /// "meet" operation of the lattice.
  ///
  /// As an example, this is used when figuring out the required availability
  /// for a type that references multiple nominal decls.
  void intersectWith(const AvailabilityContext &other) {
    OSVersion.intersectWith(other.getOSVersion());
  }

  /// Produces an over-approximation of the intersection of the two
  /// availability contexts.
  ///
  /// That is, if the intersection can't be represented exactly, prefer
  /// treating some invalid deployment environments as available.
  ///
  /// As an example, this is used for the true branch of `#available`.
  void constrainWith(const AvailabilityContext &other) {
    OSVersion.constrainWith(other.getOSVersion());
  }

  /// Produces an over-approximation of the union of two availability contexts.
  ///
  /// That is, if the union can't be represented exactly, prefer treating
  /// some invalid deployment environments as available. This is the "join"
  /// operation of the lattice.
  ///
  /// As an example, this is used for the else branch of a conditional with
  /// multiple `#available` checks.
  void unionWith(const AvailabilityContext &other) {
    OSVersion.unionWith(other.getOSVersion());
  }

  bool isAvailableAsSPI() const { return SPI && *SPI; }
};

class AvailabilityInference {
public:
  /// Infers the common availability required to access an array of
  /// declarations and adds attributes reflecting that availability
  /// to ToDecl.
  static void
  applyInferredAvailableAttrs(Decl *ToDecl,
                              ArrayRef<const Decl *> InferredFromDecls,
                              ASTContext &Context);

  static AvailabilityContext inferForType(Type t);

  /// Returns the context where a declaration is available
  ///  We assume a declaration without an annotation is always available.
  static AvailabilityContext availableRange(const Decl *D, ASTContext &C);

  /// Returns the context for which the declaration
  /// is annotated as available, or None if the declaration
  /// has no availability annotation.
  static std::optional<AvailabilityContext>
  annotatedAvailableRange(const Decl *D, ASTContext &C);

  static AvailabilityContext
  annotatedAvailableRangeForAttr(const SpecializeAttribute *attr,
                                 ASTContext &ctx);
};

} // end namespace stone

#endif