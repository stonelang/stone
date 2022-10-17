#ifndef STONE_SYNTAX_TYPELOC_H
#define STONE_SYNTAX_TYPELOC_H

#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/Types.h"

#include "llvm/ADT/PointerIntPair.h"

namespace stone {
namespace syn {

class TypeRep;
class SyntaxContext;

/// Example: int a = 0;  The start loc is i and the range is i ...t
class alignas(1 << TypeRepAlignInBits) TypeLoc {

  // TODO: This may actually be QualType instead of jus Type
  // But since QualType implements Type, it may not matter.
  Type ty;
  TypeRep *tyRep = nullptr;

public:
  TypeLoc() {}
  TypeLoc(TypeRep *tyRep) : tyRep(tyRep) {}
  TypeLoc(TypeRep *tyRep, Type ty) : tyRep(tyRep) { SetType(ty); }
  // TypeLoc(TypeRep *tyRep, QualType ty) : tyRep(tyRep) { SetType(ty); }

  bool WasValidated() const { return !ty.IsNull(); }
  bool IsError() const;

  // FIXME: We generally shouldn't need to build TypeLocs without a location.
  static TypeLoc WithoutLoc(Type ty) {
    TypeLoc result;
    result.ty = ty;
    return result;
  }

  /// Get the representative location of this type, for diagnostic
  /// purposes.
  /// This location is not necessarily the start location of the TypeRep.
  SrcLoc GetLoc() const;
  SrcRange GetSrcRange() const;
  bool HasLoc() const { return tyRep != nullptr; }

  TypeRep *GetTypeRep() const { return tyRep; }
  Type GetType() const { return ty; }

  bool IsNull() const { return GetType().IsNull() && tyRep == nullptr; }
  void SetType(Type ty);

  friend llvm::hash_code hash_value(const TypeLoc &owner) {
    return llvm::hash_combine(owner.ty.GetPtr(), owner.tyRep);
  }

  friend bool operator==(const TypeLoc &lhs, const TypeLoc &rhs) {
    return lhs.ty.GetPtr() == rhs.ty.GetPtr() && lhs.tyRep == rhs.tyRep;
  }

  friend bool operator!=(const TypeLoc &lhs, const TypeLoc &rhs) {
    return !(lhs == rhs);
  }
};

// class alignas(1 << TypeRepAlignInBits) QualTypeLoc {
//   TypeLoc typeLoc;
// public:

//   QualTypeLoc(TypeLoc typeLoc) : typeLoc(typeLoc) {}
// };

} // namespace syn
} // namespace stone

#endif
