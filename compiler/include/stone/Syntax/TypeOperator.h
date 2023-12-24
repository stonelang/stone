#ifndef STONE_SYNTAX_TYPEOPERATOR_H
#define STONE_SYNTAX_TYPEOPERATOR_H

#include "stone/Syntax/ASTAllocation.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {

enum class TypeOperatorKind : UInt8 {
  None = 0,
  New,
  Delete,
  Default,
};
class TypeOperator {
  SrcLoc loc;
  TypeOperatorKind kind;

public:
  TypeOperator(TypeOperatorKind kind, SrcLoc inputLoc)
      : kind(kind), loc(inputLoc) {}

public:
  TypeOperatorKind GetKind() const { return kind; }
  SrcLoc GetLoc() { return loc; }
};

class NewTypeOperator final : public TypeOperator {
public:
  NewTypeOperator(SrcLoc loc) : TypeOperator(TypeOperatorKind::New, loc) {}

public:
  static NewTypeOperator Create(SrcLoc loc);
};

class DeleteTypeOperator final : public TypeOperator {
public:
  DeleteTypeOperator(SrcLoc loc)
      : TypeOperator(TypeOperatorKind::Delete, loc) {}

public:
  static DeleteTypeOperator Create(SrcLoc loc);
};

class TypeOperatorCollector final {
public:
  /// This holds each type-patter that the type-specifer includes as it is
  /// parsed.  This is pushed from the type out, which means that element
  /// #0 will be the most closely bound to the type, and
  /// ops.back() will be the least closely bound to the type.
  llvm::SmallVector<TypeOperator, 8> typeOperators;

  /// If this Declarator declares a template, its template parameter lists.
  // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;
public:
  TypeOperatorCollector();

private:
  void AddTypeOperator(const TypeOperator typeOperator) {
    typeOperators.push_back(typeOperator);
  }

public:
  void AddNew(SrcLoc loc);
  void AddDelete(SrcLoc loc);
  bool HasAny() { return typeOperators.size() > 0; }

  // void Apply();
  // void Verify();
};

} // namespace stone
#endif
