#ifndef STONE_AST_TYPEQUALIFIER_H
#define STONE_AST_TYPEQUALIFIER_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/StringRef.h"

namespace stone {
  class ASTContext;
  class Type;
  class QualType;

struct FastTypeQualifierFlags final {
  FastTypeQualifierFlags() = delete;
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

class FastTypeQualifiers {

  UInt32 qualifiers = 0;
public:
  FastTypeQualifiers() : qualifiers(0) {}

public:
  bool HasConst() const { return qualifiers & FastTypeQualifierFlags::Const; }
  bool HasConstOnly() const { return qualifiers == FastTypeQualifierFlags::Const; }
  void RemoveConst() { qualifiers &= ~FastTypeQualifierFlags::Const; }
  void AddConst() {
    qualifiers |= FastTypeQualifierFlags::Const;
  }

public:
  bool HasImmutable() const {
    return qualifiers & FastTypeQualifierFlags::Immutable;
  }
  bool HasImmutableOnly() const {
    return qualifiers == FastTypeQualifierFlags::Immutable;
  }
  void RemoveImmutable() { qualifiers &= ~FastTypeQualifierFlags::Immutable; }
  void AddImmutable() {
    qualifiers |= FastTypeQualifierFlags::Immutable;
  }
public:
  bool HasMutable() const { return qualifiers & FastTypeQualifierFlags::Mutable; }
  bool HasMutableOnly() const {
    return qualifiers == FastTypeQualifierFlags::Mutable;
  }
  void RemoveMutable() { qualifiers &= ~FastTypeQualifierFlags::Mutable; }
  void AddMutable() {
    qualifiers |= FastTypeQualifierFlags::Mutable;
  }

public:
  bool HasRestrict() const { return qualifiers & FastTypeQualifierFlags::Restrict; }
  bool HasRestrictOnly() const {
    return qualifiers == FastTypeQualifierFlags::Restrict;
  }
  void RemoveRestrict() { qualifiers &= ~FastTypeQualifierFlags::Restrict; }
  void AddRestrict() {
    qualifiers |= FastTypeQualifierFlags::Restrict;
  }

public:
  bool HasVolatile() const { return qualifiers & FastTypeQualifierFlags::Volatile; }
  bool HasVolatileOnly() const {
    return qualifiers == FastTypeQualifierFlags::Volatile;
  }
  void RemoveVolatile() { qualifiers &= ~FastTypeQualifierFlags::Volatile; }
  void AddVolatile() {
    qualifiers |= FastTypeQualifierFlags::Volatile;
  }

public:
  bool HasPure() const { return qualifiers & FastTypeQualifierFlags::Pure; }
  bool HasPureOnly() const { return qualifiers == FastTypeQualifierFlags::Pure; }
  void RemovePure() { qualifiers &= ~FastTypeQualifierFlags::Pure; }
  void AddPure() {
    qualifiers |= FastTypeQualifierFlags::Pure;
  }

public:
  bool HasDelete() const { return qualifiers & FastTypeQualifierFlags::Delete; }
  bool HasDeleteOnly() const {
    return qualifiers == FastTypeQualifierFlags::Delete;
  }
  void RemoveDelete() { qualifiers &= ~FastTypeQualifierFlags::Delete; }
  void AddDelete() {
    qualifiers |= FastTypeQualifierFlags::Delete;
  }
  
public:
  bool HasDefault() const { return qualifiers & FastTypeQualifierFlags::Default; }
  bool HasDefaultOnly() const {
    return qualifiers == FastTypeQualifierFlags::Default;
  }
  void RemoveDefault() { qualifiers &= ~FastTypeQualifierFlags::Pure; }
  void AddDefault() {
    qualifiers |= FastTypeQualifierFlags::Default;
  }

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
  FastTypeQualifiers fastQuals;

public:
  TypeQualifierCollector();

public:
  void AddConst(SrcLoc loc = SrcLoc()) { constLoc = loc; fastQuals.AddConst(); }
  SrcLoc GetConst() { return constLoc; }
  bool HasConst() { return constLoc.isValid(); }

public:
  void AddImmutable(SrcLoc loc = SrcLoc()) { immutableLoc = loc; fastQuals.AddImmutable();}
  SrcLoc GetImmutable() { return immutableLoc; }
  bool HasImmutable() { return immutableLoc.isValid(); }

public:
  void AddMutable(SrcLoc loc = SrcLoc()) { mutableLoc = loc; fastQuals.AddMutable();}
  SrcLoc GetMutable() { return mutableLoc; }
  bool HasMutable() { return mutableLoc.isValid(); }

public:
  void AddRestrict(SrcLoc loc = SrcLoc()) { restrictLoc = loc; fastQuals.AddRestrict(); }
  SrcLoc GetRestrict() { return restrictLoc; }
  bool HasRestrict() { return restrictLoc.isValid(); }

public:
  void AddVolatile(SrcLoc loc = SrcLoc()) { volatileLoc = loc; fastQuals.AddVolatile();}
  SrcLoc GetVolatile() { return volatileLoc; }
  bool HasVolatile() { return volatileLoc.isValid(); }

public:
  void AddPure(SrcLoc loc = SrcLoc()) { pureLoc = loc; fastQuals.AddPure();}
  SrcLoc GetPure() { return pureLoc; }
  bool HasPure() { return pureLoc.isValid(); }

public:
  FastTypeQualifiers& GetFastQuals() { return fastQuals;}

public:
  QualType Apply(const stone::ASTContext &astContext, QualType ty) const;
  QualType Apply(const stone::ASTContext &astContext, const Type *ty) const;
};

} // namespace stone

#endif
