#ifndef STONE_SYNTAX_TYPEQUALIFIER_H
#define STONE_SYNTAX_TYPEQUALIFIER_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
class Type;
class QualType;

class TypeQualifierCollector final {

  SrcLoc constLoc;
  SrcLoc pureLoc;
  SrcLoc immutableLoc;
  SrcLoc mutableLoc;

public:
  TypeQualifierCollector()
      : constLoc(SrcLoc()), pureLoc(SrcLoc()), immutableLoc(SrcLoc()),
        mutableLoc(SrcLoc()) {}

public:
  bool HasConst() const { return constLoc.isValid(); }
  bool IsConst() const {
    return (HasConst() && !HasImmutable() && !HasMutable() && !HasPure());
  }
  void RemoveConst() { constLoc = SrcLoc(); }
  void AddConst(SrcLoc loc) { constLoc = loc; }
  SrcLoc GetConst() { return constLoc; }

public:
  bool HasImmutable() const { return immutableLoc.isValid(); }
  bool IsImmutable() const {
    return (HasImmutable() && !HasConst() && !HasMutable() && !HasPure());
  }
  void RemoveImmutable() { immutableLoc = SrcLoc(); }
  void AddImmutable(SrcLoc loc) { immutableLoc = loc; }
  SrcLoc GetImmutable() { return immutableLoc; }

public:
  bool HasMutable() const { return mutableLoc.isValid(); }
  bool IsMutable() const {
    return (HasMutable() && !HasConst() && !HasImmutable() && !HasPure());
  }
  void RemoveMutable() { mutableLoc = SrcLoc(); }
  void AddMutable(SrcLoc loc) { mutableLoc = loc; }
  SrcLoc GetMutable() { return mutableLoc; }

public:
  bool HasPure() const { pureLoc.isValid(); }
  bool IsPure() const {
    return (HasPure() && !HasConst() && !HasImmutable() && !HasMutable());
  }
  void RemovePure() { pureLoc = SrcLoc(); }
  void AddPure(SrcLoc loc) { pureLoc = loc; }
  SrcLoc GetPure() { return pureLoc; }

public:
  bool HasAny() {
    return (HasConst() || HasImmutable() || HasMutable() || HasPure());
  }
  void ClearQualifiers() {
    constLoc = SrcLoc();
    pureLoc = SrcLoc();
    immutableLoc = SrcLoc();
    mutableLoc = SrcLoc();
  }

public:
  /// Apply the collected qualifiers to the given type.
  Type Apply(TypeBase *typePtr);

  /// Apply the collected qualifiers to the given type.
  void Apply(Type &ty);
};

} // namespace stone

#endif
