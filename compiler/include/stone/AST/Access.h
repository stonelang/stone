#ifndef STONE_AST_ACCESS_H
#define STONE_AST_ACCESS_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/AST/TypeAlignment.h"

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {

class DeclContext;

// TODO: All you need is Public and Local
enum class AccessLevel : UInt8 {
  None = 0,
  /// Limited to the scope
  Private,
  /// Limited to the current file
  File,
  /// Accessible only within module
  Internal,
  /// Open outside of module with certain restrictions
  Public,
  /// Completely open and available
  Global,
};

/// The wrapper around the outermost DeclContext from which
/// a particular declaration can be accessed.
class AccessScope final {
  /// The declaration context (if not public) along with a bit saying
  /// whether this scope is private, SPI or not.
  /// If the declaration context is set, the bit means that the scope is
  /// private or not. If the declaration context is null, the bit means that
  /// this scope is SPI or not.
  llvm::PointerIntPair<const DeclContext *, 1, bool> val;

public:
  AccessScope(const DeclContext *DC, bool isPrivate = false);

public:
  static AccessScope GetPublic() { return AccessScope(nullptr, false); }

  /// Check if private access is allowed. This is a lexical scope check in Swift
  /// 3 mode. In Swift 4 mode, declarations and extensions of the same type will
  /// also allow access.
  static bool AllowsPrivateAccess(const DeclContext *useDC,
                                  const DeclContext *sourceDC);

  /// Returns nullptr if access scope is public.
  const DeclContext *GetDeclContext() const { return val.getPointer(); }

  bool operator==(AccessScope RHS) const { return val == RHS.val; }
  bool operator!=(AccessScope RHS) const { return !(*this == RHS); }

  bool HasEqualDeclContextWith(AccessScope RHS) const {
    return GetDeclContext() == RHS.GetDeclContext();
  }

  bool IsPublic() const { return !val.getPointer(); }
  bool IsPrivate() const { return val.getPointer() && val.getInt(); }
  bool IsFileScope() const;
  bool IsInternal() const;
  bool IsGlobal() const;

  /// Returns true if this is a child scope of the specified other access scope.
  ///
  /// \see DeclContext::isChildContextOf
  bool IsChildOf(AccessScope AS) const {
    if (!IsPublic() && !AS.IsPublic()) {
      return AllowsPrivateAccess(GetDeclContext(), AS.GetDeclContext());
    }
    if (IsPublic() && AS.IsPublic()) {
      return false;
    }
    return AS.IsPublic();
  }

  /// Returns the associated access level for diagnostic purposes.
  AccessLevel GetAccessLevelForDiagnostics() const;

  /// Returns the minimum access level required to access
  /// associated DeclContext for diagnostic purposes.
  AccessLevel GetRequiredAccessForDiagnostics() const {
    return IsFileScope() ? AccessLevel::File : GetAccessLevelForDiagnostics();
  }

  /// Returns the narrowest access scope if this and the specified access scope
  /// have common intersection, or None if scopes don't intersect.
  const std::optional<AccessScope>
  IntersectWith(AccessScope accessScope) const {
    if (HasEqualDeclContextWith(accessScope)) {
      if (IsPrivate()) {
        return *this;
      }
      return accessScope;
    }
    if (IsChildOf(accessScope)) {
      return *this;
    }
    if (accessScope.IsChildOf(*this)) {
      return accessScope;
    }

    return std::nullopt;
  }
};
} // namespace stone
#endif