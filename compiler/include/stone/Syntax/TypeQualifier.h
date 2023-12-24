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

class TypeQualifierCollector {

  unsigned qualifiers = 0;

  SrcLoc constLoc;
  SrcLoc pureLoc;
  SrcLoc immutableLoc;
  SrcLoc mutableLoc;

  enum ID : unsigned {
    None = 1 << 0,
    Const = 1 << 1,
    Immutable = 1 << 2,
    Mutable = 1 << 3,
    Pure = 1 << 4,
  };

public:
  TypeQualifierCollector()
      : qualifiers(0), constLoc(SrcLoc()), pureLoc(SrcLoc()),
        immutableLoc(SrcLoc()), mutableLoc(SrcLoc()) {}

public:
  bool HasConst() const { return qualifiers & TypeQualifierCollector::Const; }
  bool IsConst() const { return qualifiers == TypeQualifierCollector::Const; }
  void RemoveConst() { qualifiers &= ~TypeQualifierCollector::Const; }
  void AddConst(SrcLoc loc = SrcLoc()) {
    constLoc = loc;
    qualifiers |= TypeQualifierCollector::Const;
  }
  SrcLoc GetConstLoc() { return constLoc; }

public:
  bool HasImmutable() const {
    return qualifiers & TypeQualifierCollector::Immutable;
  }
  bool IsImmutable() const {
    return qualifiers == TypeQualifierCollector::Immutable;
  }
  void RemoveImmutable() { qualifiers &= ~TypeQualifierCollector::Immutable; }
  void AddImmutable(SrcLoc loc = SrcLoc()) {
    immutableLoc = loc;
    qualifiers |= TypeQualifierCollector::Immutable;
  }
  SrcLoc GetImmutableLoc() { return immutableLoc; }

public:
  bool HasMutable() const {
    return qualifiers & TypeQualifierCollector::Mutable;
  }
  bool IsMutable() const {
    return qualifiers == TypeQualifierCollector::Mutable;
  }
  void RemoveMutable() { qualifiers &= ~TypeQualifierCollector::Mutable; }
  void AddMutable(SrcLoc loc = SrcLoc()) {
    mutableLoc = loc;
    qualifiers |= TypeQualifierCollector::Mutable;
  }
  SrcLoc GetMutableLoc() { return mutableLoc; }

public:
  bool HasPure() const { return qualifiers & TypeQualifierCollector::Pure; }
  bool IsPure() const { return qualifiers == TypeQualifierCollector::Pure; }
  void RemovePure() { qualifiers &= ~TypeQualifierCollector::Pure; }
  void AddPure(SrcLoc loc = SrcLoc()) {
    pureLoc = loc;
    qualifiers |= TypeQualifierCollector::Pure;
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

} // namespace stone

#endif
