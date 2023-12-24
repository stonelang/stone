#ifndef STONE_SYNTAX_TYPEQUALIFIER_H
#define STONE_SYNTAX_TYPEQUALIFIER_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {

// NOTES:
// Array types are considered to have the same cv-qualification as their element
// types.

struct TypeQualifierFlags final {

  TypeQualifierFlags() = delete;

  enum ID : unsigned {
    None = 1 << 0,
    Const = 1 << 1,
    Immutable = 1 << 2,
    Mutable = 1 << 3,
    Pure = 1 << 4,
  };
};

class TypeQualifierList {

  unsigned qualifiers = 0;

  SrcLoc constLoc;
  SrcLoc pureLoc;
  SrcLoc immutableLoc;
  SrcLoc mutableLoc;

public:
  TypeQualifierList()
      : qualifiers(0), constLoc(SrcLoc()), pureLoc(SrcLoc()),
        immutableLoc(SrcLoc()), mutableLoc(SrcLoc()) {}

public:
  bool HasConst() const { return qualifiers & TypeQualifierFlags::Const; }
  bool IsConst() const { return qualifiers == TypeQualifierFlags::Const; }
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
  bool IsImmutable() const {
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
  bool IsMutable() const { return qualifiers == TypeQualifierFlags::Mutable; }
  void RemoveMutable() { qualifiers &= ~TypeQualifierFlags::Mutable; }
  void AddMutable(SrcLoc loc = SrcLoc()) {
    mutableLoc = loc;
    qualifiers |= TypeQualifierFlags::Mutable;
  }
  SrcLoc GetMutableLoc() { return mutableLoc; }

public:
  bool HasPure() const { return qualifiers & TypeQualifierFlags::Pure; }
  bool IsPure() const { return qualifiers == TypeQualifierFlags::Pure; }
  void RemovePure() { qualifiers &= ~TypeQualifierFlags::Pure; }
  void AddPure(SrcLoc loc = SrcLoc()) {
    pureLoc = loc;
    qualifiers |= TypeQualifierFlags::Pure;
  }
  SrcLoc GetPureLoc() { return pureLoc; }

public:
  bool HasAny() {
    return (HasConst() || HasImmutable() || HasMutable() || HasPure());
  }

  void Reset() {
    qualifiers = 0;
    constLoc = SrcLoc();
    pureLoc = SrcLoc();
    immutableLoc = SrcLoc();
    mutableLoc = SrcLoc();
  }
};

class TypeQualifierCollector final : public TypeQualifierList {
public:
  TypeQualifierCollector();

public:
  TypeQualifierList *GetTypeQualifiers() { HasAny() ? this : nullptr; }
};
} // namespace stone

#endif
