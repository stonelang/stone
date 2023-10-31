#ifndef STONE_ASTTYPELOC_H
#define STONE_ASTTYPELOC_H

#include "stone/AST/TypeAlignment.h"
#include "stone/AST/Types.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/PointerIntPair.h"

namespace stone {
namespace ast {

class TypeRep;
class ASTContext;

/// Example: int a = 0;  The start loc is i and the range is i ...t
class alignas(1 << TypeAlignInBits) TypeLoc {
  // TODO: This may actually be QualType instead of jus Type
  // But since QualType implements Type, it may not matter.
  Type ty;

public:
  TypeLoc() {}
  TypeLoc(Type ty) : ty(ty) {}
  // TypeLoc(TypeRep *tyRep, QualType ty) : tyRep(tyRep) { SetType(ty); }

  bool WasValidated() const { return !ty.IsNull(); }
  bool IsError() const;

  /// Get the representative location of this type, for diagnostic
  /// purposes.
  /// This location is not necessarily the start location of the TypeRep.
  SrcLoc GetLoc() const;
  SrcRange GetSrcRange() const;

  // bool HasLoc() const { return tyRep != nullptr; }

  Type GetType() const { return ty; }

  // bool IsNull() const { return GetType().IsNull() && tyRep == nullptr; }
  void SetType(Type ty);
};

} // namespace ast
} // namespace stone

#endif
