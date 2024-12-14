#ifndef STONE_AST_VSIBILITY_H
#define STONE_AST_VSIBILITY_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {

class DeclContext;
enum class VisibilityKind : UInt8 {
  None = 0,
  Public,
  Private,
  Internal,
  Global,
  File,
};

/// The wrapper around the outermost DeclContext from which
/// a particular declaration can be accessed.
class VisibilityScope final {
  /// The declaration context (if not public) along with a bit saying
  /// whether this scope is private, SPI or not.
  /// If the declaration context is set, the bit means that the scope is
  /// private or not. If the declaration context is null, the bit means that
  /// this scope is SPI or not.
  llvm::PointerIntPair<const DeclContext *, 1, bool> val;

public:
  VisibilityScope(const DeclContext *DC, bool isPrivate = false);

public:
  static VisibilityScope GetPublic() { return VisibilityScope(nullptr, false); }

  /// Check if private access is allowed. This is a lexical scope check in Swift
  /// 3 mode. In Swift 4 mode, declarations and extensions of the same type will
  /// also allow access.
  static bool AllowsPrivateVisibility(const DeclContext *useDC,
                                      const DeclContext *sourceDC);

  /// Returns nullptr if access scope is public.
  const DeclContext *GetDeclContext() const { return val.getPointer(); }

  bool operator==(VisibilityScope RHS) const { return val == RHS.val; }
  bool operator!=(VisibilityScope RHS) const { return !(*this == RHS); }

  bool HasEqualDeclContextWith(VisibilityScope RHS) const {
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
  bool IsChildOf(VisibilityScope vs) const {
    if (!IsPublic() && !vs.IsPublic()) {
      return AllowsPrivateVisibility(GetDeclContext(), vs.GetDeclContext());
    }
    if (IsPublic() && vs.IsPublic()) {
      return false;
    }
    return vs.IsPublic();
  }

  /// Returns the associated access kind for diagnostic purposes.
  VisibilityKind GetVisibilityKindForDiagnostics() const;

  /// Returns the minimum access kind required to access
  /// associated DeclContext for diagnostic purposes.
  VisibilityKind GetRequiredVisibilityForDiagnostics() const {
    return IsFileScope() ? VisibilityKind::File
                         : GetVisibilityKindForDiagnostics();
  }

  /// Returns the narrowest access scope if this and the specified access scope
  /// have common intersection, or None if scopes don't intersect.
  const std::optional<VisibilityScope>
  IntersectWith(VisibilityScope visibilityScope) const {
    if (HasEqualDeclContextWith(visibilityScope)) {
      if (IsPrivate()) {
        return *this;
      }
      return visibilityScope;
    }
    if (IsChildOf(visibilityScope)) {
      return *this;
    }
    if (visibilityScope.IsChildOf(*this)) {
      return visibilityScope;
    }

    return std::nullopt;
  }
};

} // namespace stone
#endif