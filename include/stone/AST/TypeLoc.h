#ifndef STONE_AST_TYPELOC_H
#define STONE_AST_TYPELOC_H

#include "stone/AST/QualType.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/PointerIntPair.h"

namespace stone {

enum class TypeLocKind : uint8 {
#define TYPE(id, parent) id,
#define LAST_TYPE(id) Last_Type = id,
#define TYPE_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/AST/TypeKind.def"
};

/// Example: int a = 0;  The start loc is i and the range is i ...t
class alignas(1 << TypeAlignInBits) TypeLoc {
  // TODO: This may actually be QualType instead of jus Type
  // But since QualType implements Type, it may not matter.
  QualType ty;

public:
  TypeLoc() {}
  TypeLoc(QualType ty) : ty(ty) {}
  // TypeLoc(TypeRep *tyRep, QualType ty) : tyRep(tyRep) { SetType(ty); }

  bool WasValidated() const { return !ty.IsNull(); }
  bool IsError() const;

  /// Get the representative location of this type, for diagnostic
  /// purposes.
  /// This location is not necessarily the start location of the TypeRep.
  SrcLoc GetLoc() const;
  SrcRange GetSrcRange() const;

  // bool HasLoc() const { return tyRep != nullptr; }

  QualType GetType() const { return ty; }

  // bool IsNull() const { return GetType().IsNull() && tyRep == nullptr; }
  void SetType(QualType ty);
};

} // namespace stone

#endif
