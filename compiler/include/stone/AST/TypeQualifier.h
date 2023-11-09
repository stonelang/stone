#ifndef STONE_AST_TYPEQUALIFIER_H
#define STONE_AST_TYPEQUALIFIER_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/StringRef.h"

namespace stone {
class ASTContext;
class Type;
class QualType;

enum class TypeQualifierKind : UInt8 {
  None = 0,
  Const,
  Own,
  Mutable,
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

  SrcLoc constLoc;
  SrcLoc ownLoc;
  SrcLoc mutableLoc;

public:
  TypeQualifierCollector();

public:
  void AddConst(SrcLoc loc = SrcLoc()) {
    constLoc = loc;
  }
  SrcLoc GetConst() { return constLoc; }
  bool HasConst() { return constLoc.isValid(); }

public:
  void AddMutable(SrcLoc loc = SrcLoc()) {mutableLoc = loc;}
  SrcLoc GetMutable() { return mutableLoc; }
  bool HasMutable() { return mutableLoc.isValid(); }

public:
  void AddOwn(SrcLoc loc = SrcLoc()) {mutableLoc = loc;}
  SrcLoc GetOwn() { return mutableLoc; }
  bool HasOwn() { return mutableLoc.isValid(); }
};

} // namespace stone

#endif
