#ifndef STONE_ASTTYPEQUALIFIER_H
#define STONE_ASTTYPEQUALIFIER_H

#include "stone/AST/TypeAlignment.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

#include <bitset>

namespace stone {
namespace ast {

// NOTES:
// Array types are considered to have the same cv-qualification as their element
// types.

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

  // TODO: Remove delete -- it is an operator
  SrcLoc deleteLoc;
  SrcLoc defaultLoc;

public:
  TypeQualifierList()
      : qualifiers(0), constLoc(SrcLoc()), restrictLoc(SrcLoc()) {}

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
  void ClearAll() { qualifiers = 0; }
};

enum class TypeQualifierKind : UInt8 {
  None = 0,
  Const,
  Restrict,
  Volatile,
  Unaligned,
  Immutable,
  Mutable,
  Pure,
};
class TypeQualifier final {
  SrcLoc loc;
  TypeQualifierKind kind;

public:
  TypeQualifier(TypeQualifierKind kind) : loc(SrcLoc()), kind(kind) {}

public:
  void AddLoc(SrcLoc inputLoc = SrcLoc()) { loc = inputLoc; }
  void Clear() { loc = SrcLoc(); }
  bool IsValid() { return (loc.isValid() && kind != TypeQualifierKind::None); }

public:
  TypeQualifierKind GetKind() { return kind; }
  llvm::StringRef GetName() const;
};

class TypeQualifierCollector final {

  class QualType;

  SrcLoc constLoc;
  SrcLoc restrictLoc;
  SrcLoc volatileLoc;
  SrcLoc pureLoc;
  SrcLoc immutableLoc;
  SrcLoc mutableLoc;

public:
  TypeQualifierCollector();

public:
  void AddConst(SrcLoc loc = SrcLoc()) { constLoc = loc; }
  SrcLoc GetConstLoc() { return constLoc; }

public:
  void AddImmutable(SrcLoc loc = SrcLoc()) { immutableLoc = loc; }
  SrcLoc GetImmutableLoc() { return immutableLoc; }

public:
  void AddMutable(SrcLoc loc = SrcLoc()) { mutableLoc = loc; }
  SrcLoc GetMutableLoc() { return mutableLoc; }

public:
  void AddRestrict(SrcLoc loc = SrcLoc()) { restrictLoc = loc; }
  SrcLoc GetRestrictLoc() { return restrictLoc; }

public:
  void AddVolatile(SrcLoc loc = SrcLoc()) { volatileLoc = loc; }
  SrcLoc GetVolatileLoc() { return volatileLoc; }

public:
  void AddPure(SrcLoc loc = SrcLoc()) { pureLoc = loc; }
  SrcLoc GetPureLoc() { return pureLoc; }

public:
  QualType Apply(const ast::ASTContext &astContext, QualType ty) const;
  QualType Apply(const ast::ASTContext &astContext, const Type *ty) const;
};
} // namespace ast
} // namespace stone

#endif
