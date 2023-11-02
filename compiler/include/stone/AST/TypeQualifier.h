#ifndef STONE_ASTTYPEQUALIFIER_H
#define STONE_ASTTYPEQUALIFIER_H

#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/StringRef.h"

namespace stone {
namespace ast {

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
