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
  enum ID : unsigned {
    None = 0,
    Const = 1 << 1,
    Restrict = 1 << 2,
    Volatile = 1 << 3,
    Unaligned = 1 << 4,
    Immutable = 1 << 5,
    Mutable = 1 << 6,
    Pure = 1 << 7,
    Delete = 1 << 8,
    Default = 1 << 9,
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
  SrcLoc deleteLoc;
  SrcLoc defaultLoc;

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
  bool HasPure() const { return qualifiers & TypeQualifierFlags::Pure; }
  bool HasPureOnly() const { return qualifiers == TypeQualifierFlags::Pure; }
  void RemovePure() { qualifiers &= ~TypeQualifierFlags::Pure; }
  void AddPure(SrcLoc loc = SrcLoc()) {
    pureLoc = loc;
    qualifiers |= TypeQualifierFlags::Pure;
  }
  SrcLoc GetPureLoc() { return pureLoc; }

public:
  bool HasDelete() const { return qualifiers & TypeQualifierFlags::Delete; }
  bool HasDeleteOnly() const {
    return qualifiers == TypeQualifierFlags::Delete;
  }
  void RemoveDelete() { qualifiers &= ~TypeQualifierFlags::Delete; }
  void AddDelete(SrcLoc loc = SrcLoc()) {
    deleteLoc = loc;
    qualifiers |= TypeQualifierFlags::Delete;
  }
  SrcLoc GetDeleteLoc() { return pureLoc; }

public:
  bool HasDefault() const { return qualifiers & TypeQualifierFlags::Default; }
  bool HasDefaultOnly() const {
    return qualifiers == TypeQualifierFlags::Default;
  }
  void RemoveDefault() { qualifiers &= ~TypeQualifierFlags::Pure; }
  void AddDefault(SrcLoc loc = SrcLoc()) {
    defaultLoc = loc;
    qualifiers |= TypeQualifierFlags::Default;
  }
  SrcLoc GetDefaultLoc() { return pureLoc; }

public:
  bool HasAny() {
    return (HasConst() || HasRestrict() || HasVolatile() || HasImmutable() ||
            HasMutable() || HasPure() || HasDelete() || HasDefault());
  }
  bool HasAll() {
    return (HasConst() && HasRestrict() && HasVolatile() && HasImmutable() &&
            HasMutable() && HasPure());
  }
};

class TypeQualifierCollector final : public TypeQualifierList {
public:
  TypeQualifierCollector();

public:
  TypeQualifierList *GetTypeQualifiers() { HasAny() ? this : nullptr; }
};
} // namespace syn
} // namespace stone

#endif
