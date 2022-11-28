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

struct TypeQualifierFlags final {
  TypeQualifierFlags() = delete;
  enum ID : UInt8 {
    None = 0,
    Const = 1 << 1,
    Restrict = 1 << 2,
    Volatile = 1 << 3,
    Unaligned = 1 << 4,
    Immutable = 1 << 5,
    Mutable = 1 << 6,
  };
};

class TypeQualifierList {

  UInt32 qualifiers = 0;
  SrcLoc constLoc;
  SrcLoc restrictLoc;
  SrcLoc volatileLoc;
  SrcLoc pureLoc;
  SrcLoc immutableLoc;
  SrcLoc mutableLoc;

public:
  TypeQualifierList() {}

public:
  bool HasConst() const { return qualifiers & TypeQualifierFlags::Const; }
  bool HasConstOnly() const { return qualifiers == TypeQualifierFlags::Const; }
  void RemoveConst() { qualifiers &= ~TypeQualifierFlags::Const; }
  void AddConst(SrcLoc loc = SrcLoc()) {
    constLoc = loc;
    qualifiers |= TypeQualifierFlags::Const;
  }
  SrcLoc GetConstLoc() { return constLoc; }

public:
  bool HasImmutable() const {
    return qualifiers & TypeQualifierFlags::Immutable;
  }
  bool HasImmutableOnly() const {
    return qualifiers == TypeQualifierFlags::Immutable;
  }
  void RemoveImmutable() { qualifiers &= ~TypeQualifierFlags::Immutable; }
  void AddImmutable(SrcLoc loc = SrcLoc()) {
    immutableLoc = loc;
    qualifiers |= TypeQualifierFlags::Immutable;
  }
  SrcLoc GetImmutableLoc() { return immutableLoc; }

public:
  bool HasMutable() const { return qualifiers & TypeQualifierFlags::Mutable; }
  bool HasMutableOnly() const {
    return qualifiers == TypeQualifierFlags::Mutable;
  }
  void RemoveMutable() { qualifiers &= ~TypeQualifierFlags::Mutable; }
  void AddMutable(SrcLoc loc = SrcLoc()) {
    mutableLoc = loc;
    qualifiers |= TypeQualifierFlags::Mutable;
  }
  SrcLoc GetMutableLoc() { return mutableLoc; }

public:
  bool HasRestrict() const { return qualifiers & TypeQualifierFlags::Restrict; }
  bool HasRestrictOnly() const {
    return qualifiers == TypeQualifierFlags::Restrict;
  }
  void RemoveRestrict() { qualifiers &= ~TypeQualifierFlags::Restrict; }
  void AddRestrict(SrcLoc loc = SrcLoc()) {
    restrictLoc = loc;
    qualifiers |= TypeQualifierFlags::Restrict;
  }
  SrcLoc GetRestrictLoc() { return restrictLoc; }

public:
  bool HasVolatile() const { return qualifiers & TypeQualifierFlags::Volatile; }
  bool HasVolatileOnly() const {
    return qualifiers == TypeQualifierFlags::Volatile;
  }
  void RemoveVolatile() { qualifiers &= ~TypeQualifierFlags::Volatile; }
  void AddVolatile(SrcLoc loc = SrcLoc()) {
    volatileLoc = loc;
    qualifiers |= TypeQualifierFlags::Volatile;
  }
  SrcLoc GetVolatileLoc() { return volatileLoc; }

public:
  bool HasAny() {
    return (HasConst() || HasRestrict() || HasVolatile() || HasImmutable() ||
            HasMutable());
  }
  bool HasAll() {
    return (HasConst() && HasRestrict() && HasVolatile() && HasImmutable() &&
            HasMutable());
  }
};

class TypeQualifierCollector final : public TypeQualifierList {
public:
  TypeQualifierCollector();
};
} // namespace syn
} // namespace stone

#endif
