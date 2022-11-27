#ifndef STONE_SYNTAX_TYPEQUALIFIER_H
#define STONE_SYNTAX_TYPEQUALIFIER_H

#include "stone/Basic/STDTypeAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
namespace syn {

 struct TypeQualifierFlags final  {
  enum ID : UInt8 {
    None = 1 << 0,
    Const = 1 << 1,
    Restrict = 1 << 2,
    Volatile = 1 << 3,
    Unaligned = 1 << 4,
    Immutable = 1 << 5,
    Mutable = 1 << 6,
  };
};

enum class TypeQualifierKind : UInt8 {
  None = 0,
  Const,
  Restrict,
  Volatile,
  Unaligned,
  Immutable,
  Mutable,
};

class alignas(1 << TypeAlignInBits) TypeQualifier {
  SrcLoc loc;
  TypeQualifierKind kind;

public:
  TypeQualifier(TypeQualifierKind kind, SrcLoc loc) : kind(kind), loc(loc) {}

public:
  TypeQualifierKind GetKind() { return kind; }
  SrcLoc GetLoc() { return loc; }
};

class ConstTypeQualifier final : public TypeQualifier {
public:
  ConstTypeQualifier(SrcLoc loc)
      : TypeQualifier(TypeQualifierKind::Const, loc) {}

public:
  static ConstTypeQualifier Create(SrcLoc loc);
};
class RestrictTypeQualifier final : public TypeQualifier {
public:
  RestrictTypeQualifier(SrcLoc loc)
      : TypeQualifier(TypeQualifierKind::Restrict, loc) {}

public:
  static RestrictTypeQualifier Create(SrcLoc loc);
};
class VolatileTypeQualifier final : public TypeQualifier {
public:
  VolatileTypeQualifier(SrcLoc loc)
      : TypeQualifier(TypeQualifierKind::Volatile, loc) {}

public:
  static VolatileTypeQualifier Create(SrcLoc loc);
};

class UnalignedTypeQualifier final : public TypeQualifier {
public:
  UnalignedTypeQualifier(SrcLoc loc)
      : TypeQualifier(TypeQualifierKind::Unaligned, loc) {}

public:
  static UnalignedTypeQualifier Create(SrcLoc loc);
};

class ImmutableTypeQualifier final : public TypeQualifier {
public:
  ImmutableTypeQualifier(SrcLoc loc)
      : TypeQualifier(TypeQualifierKind::Immutable, loc) {}

public:
  static ImmutableTypeQualifier Create(SrcLoc loc);
};
class MutableTypeQualifier final : public TypeQualifier {
public:
  MutableTypeQualifier(SrcLoc loc)
      : TypeQualifier(TypeQualifierKind::Mutable, loc) {}

public:
  static MutableTypeQualifier Create(SrcLoc loc);
};

class TypeQualifierList final
    : private llvm::TrailingObjects<TypeQualifierList, TypeQualifier> {

  friend TrailingObjects;

public:
  /// No copying
  TypeQualifierList(const TypeQualifierList &) = delete;
  TypeQualifierList &operator=(const TypeQualifierList &) = delete;

public:
  TypeQualifierList(llvm::ArrayRef<TypeQualifier> qualifiers);

public:
  static TypeQualifierList *Create(llvm::ArrayRef<TypeQualifier> qualifiers,
                                   SyntaxContext &sc);
};

class TypeQualifierCollector final {
  llvm::SmallVector<TypeQualifier, 4> qualifiers;

public:
  TypeQualifierCollector() {}

private:
  void AddQualifier(const TypeQualifier qualifier) {
    qualifiers.push_back(qualifier);
  }

public:
  void AddConst(SrcLoc loc) { AddQualifier(ConstTypeQualifier::Create(loc)); }

  void AddImmutable(SrcLoc loc) {
    AddQualifier(ImmutableTypeQualifier::Create(loc));
  }
  void AddMutable(SrcLoc loc) {
    AddQualifier(MutableTypeQualifier::Create(loc));
  }
  void AddRestrict(SrcLoc loc) {
    AddQualifier(RestrictTypeQualifier::Create(loc));
  }
  void AddVolatile(SrcLoc loc) {
    AddQualifier(VolatileTypeQualifier::Create(loc));
  }

private:
  bool FindKind(TypeQualifierKind kind) const {
    for (auto qualifier : qualifiers) {
      return qualifier.GetKind() == kind;
    }
    return false;
  }

public:
  bool HasAny() { return qualifiers.size() > 0; }

public:
  bool HasConst() const { return FindKind(TypeQualifierKind::Const); }
  bool HasRestrict() const { return FindKind(TypeQualifierKind::Restrict); }
  bool HasImmutable() const { return FindKind(TypeQualifierKind::Immutable); }
  bool HasMutable() const { return FindKind(TypeQualifierKind::Mutable); }

public:
  llvm::ArrayRef<TypeQualifier> GetTypeQualifiers() { return qualifiers; }
};
} // namespace syn
} // namespace stone

#endif
