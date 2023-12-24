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
class TypeBase;

// class TypeQualifiers {
//   unsigned qualifiers = 0;
//   enum Flags : unsigned {
//     None = 1 << 0,
//     Const = 1 << 1,
//     Immutable = 1 << 2,
//     Mutable = 1 << 3,
//     Pure = 1 << 4,
//   };

// public:
//   TypeQualifiers() : qualifiers(0) {}

// public:
//   bool HasConst() const { return qualifiers & Flags::Const; }
//   bool IsConst() const { return qualifiers == Flags::Const; }
//   void RemoveConst() { qualifiers &= ~Flags::Const; }
//   void AddConst() { qualifiers |= Flags::Const; }

// public:
//   bool HasImmutable() const { return qualifiers & Flags::Immutable; }
//   bool IsImmutable() const { return qualifiers == Flags::Immutable; }
//   void RemoveImmutable() { qualifiers &= ~Flags::Immutable; }
//   void AddImmutable() { qualifiers |= Flags::Immutable; }

// public:
//   bool HasMutable() const { return qualifiers & Flags::Mutable; }
//   bool IsMutable() const { return qualifiers == Flags::Mutable; }
//   void RemoveMutable() { qualifiers &= ~Flags::Mutable; }
//   void AddMutable() { qualifiers |= Flags::Mutable; }

// public:
//   bool HasPure() const { return qualifiers & Flags::Pure; }
//   bool IsPure() const { return qualifiers == Flags::Pure; }
//   void RemovePure() { qualifiers &= ~Flags::Pure; }
//   void AddPure() { qualifiers |= Flags::Pure; }

// public:
//   bool HasAny() {
//     return (HasConst() || HasImmutable() || HasMutable() || HasPure());
//   }
//   void ClearQualifiers() { qualifiers = 0; }
// };

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
