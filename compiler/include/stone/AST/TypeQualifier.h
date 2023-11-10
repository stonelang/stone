#ifndef STONE_AST_TYPEQUALIFIER_H
#define STONE_AST_TYPEQUALIFIER_H

#include "stone/AST/InlineBitfield.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/StringRef.h"

namespace stone {

class TypeQualifiers final {
public:
  /// A single property.
  ///
  /// Note that the property polarities should be chosen so that 0 is
  /// the correct default value and bitwise-or correctly merges things.
  enum TypeQualifier : unsigned {
    /// const int x = 10 -- can cast way in certain cases
    HasConstQualifier = 0x01,
    /// perm int x = 10 -- can never be casted away under any circumstance
    HasPermQualifier = 0x02,
    /// own SomeType* ty = new SomeType();
    HasOwnQualifier = 0x04,
    //// mutable int i = 1;
    HasMutableQualifier = 0x08,
    // Total bits used
    TotalBits = HasMutableQualifier
  };
  enum { BitWidth = stone::CountBitsUsed(TypeQualifier::TotalBits) };

private:
  unsigned Bits;

public:
  TypeQualifiers() : Bits(0) {}
  TypeQualifiers(TypeQualifier qual) : Bits(qual) {}
  explicit TypeQualifiers(unsigned bits) : Bits(bits) {}

  /// Return these properties as a bitfield.
  unsigned GetBits() const { return Bits; }

  bool AddConst() { return Bits |= HasConstQualifier; }
  bool HasConst() const { return Bits & HasConstQualifier; }
  bool ClearConst() { return Bits &= ~HasConstQualifier; }

  bool AddPerm() { return Bits |= HasPermQualifier; }
  bool HasPerm() const { return Bits & HasPermQualifier; }
  bool ClearPerm() { return Bits &= ~HasPermQualifier; }

  bool AddOwn() { return Bits != HasOwnQualifier; }
  bool HasOwn() const { return Bits & HasOwnQualifier; }
  bool ClearOwn() { return Bits &= ~HasOwnQualifier; }

  bool AddMutable() { return Bits != HasMutableQualifier; }
  bool HasMutable() const { return Bits & HasMutableQualifier; }
  bool ClearMutable() { return Bits &= ~HasMutableQualifier; }

  /// Returns the set of properties present in either set.
  friend TypeQualifiers operator|(TypeQualifier lhs, TypeQualifier rhs) {
    return TypeQualifiers(unsigned(lhs) | unsigned(rhs));
  }
  friend TypeQualifiers operator|(TypeQualifiers lhs, TypeQualifiers rhs) {
    return TypeQualifiers(lhs.Bits | rhs.Bits);
  }

  /// Add any properties in the right-hand set to this set.
  TypeQualifiers &operator|=(TypeQualifiers other) {
    Bits |= other.Bits;
    return *this;
  }
  /// Restrict this to only the properties in the right-hand set.
  TypeQualifiers &operator&=(TypeQualifiers other) {
    Bits &= other.Bits;
    return *this;
  }
  /// Test for a particular property in this set.
  bool operator&(TypeQualifier qual) const { return Bits & qual; }
};

class TypeQualifierCollector final {

  SrcLoc constLoc;
  SrcLoc ownLoc;
  SrcLoc permLoc;
  SrcLoc mutableLoc;

public:
  TypeQualifierCollector();

public:
  void AddConst(SrcLoc loc = SrcLoc()) { constLoc = loc; }
  SrcLoc GetConst() { return constLoc; }
  bool HasConst() { return constLoc.isValid(); }
  bool HasConstOnly() {
    return HasConst() && (!HasMutable() || !HasOwn() || !HasPerm());
  }

public:
  void AddMutable(SrcLoc loc = SrcLoc()) { mutableLoc = loc; }
  SrcLoc GetMutable() { return mutableLoc; }
  bool HasMutable() { return mutableLoc.isValid(); }
  bool HasMutableOnly() {
    return HasMutable() && (!HasConst() || !HasOwn() || !HasPerm());
  }

public:
  void AddOwn(SrcLoc loc = SrcLoc()) { mutableLoc = loc; }
  SrcLoc GetOwn() { return mutableLoc; }
  bool HasOwn() { return mutableLoc.isValid(); }
  bool HasOwnOnly() {
    return HasOwn() && (!HasConst() || !HasMutable() || !HasPerm());
  }

public:
  void AddPerm(SrcLoc loc = SrcLoc()) { mutableLoc = loc; }
  SrcLoc GetPerm() { return mutableLoc; }
  bool HasPerm() { return mutableLoc.isValid(); }
  bool HasPermOnly() {
    return HasOwn() && (!HasConst() || !HasMutable() || !HasOwn());
  }

public:
  bool HasAny() { return (HasConst() || HasMutable() || HasOwn()); }

public:
  TypeQualifiers ComputeTypeQaulifiers();
};

} // namespace stone

#endif
