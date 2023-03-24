#ifndef STONE_SYNTAX_TYPEOPERATOR_H
#define STONE_SYNTAX_TYPEOPERATOR_H

#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
namespace syn {

enum class TypeOperatorKind : UInt8 {
  None = 0,
  New,
  Delete,
  Default,
};
class alignas(1 << TypeAlignInBits) TypeOperator
    : syn::SyntaxAllocation<TypeOperator> {
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

class TypeOperatorList final
    : private llvm::TrailingObjects<TypeOperatorList, TypeOperator> {

  friend TrailingObjects;

public:
  /// No copying
  TypeOperatorList(const TypeOperatorList &) = delete;
  TypeOperatorList &operator=(const TypeOperatorList &) = delete;

public:
  TypeOperatorList(llvm::ArrayRef<TypeOperator> ops);

public:
  static TypeOperatorList *Create(llvm::ArrayRef<TypeOperator> ops,
                                  SyntaxContext &sc);
};

class TypeOperatorCollector final {

  /// This holds each type-patter that the type-specifer includes as it is
  /// parsed.  This is pushed from the type out, which means that element
  /// #0 will be the most closely bound to the type, and
  /// ops.back() will be the least closely bound to the type.
  llvm::SmallVector<TypeOperator, 8> ops;

  /// If this Declarator declares a template, its template parameter lists.
  // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;
public:
  TypeOperatorCollector();

private:
  void AddTypeOperator(const TypeOperator thunk) {
    ops.push_back(thunk);
    // TODO:
    //  if (!EndLoc.isInvalid())
    //    SetRangeEnd(EndLoc);
  }

public:
  void AddNew(SrcLoc loc);
  void AddDelete(SrcLoc loc);

public:
  // /// int** -- the '*' toucing int
  // const TypeOperator *GetInnermostNonParenChunk() const {
  //   for (unsigned i = ops.size(), i_end = 0; i != i_end; --i) {
  //     if (ops[i - 1].GetKind() != TypeOperatorKind::Paren) {
  //       return &ops[i - 1];
  //     }
  //   }
  //   return nullptr;
  // }

  // /// int** -- the '*' farthest from int
  // const TypeOperator *GetOutermostNonParenChunk() const {
  //   for (unsigned i = 0, i_end = ops.size(); i < i_end; ++i) {
  //     if (ops[i].GetKind() != TypeOperatorKind::Paren) {
  //       return &ops[i];
  //     }
  //   }
  //   return nullptr;
  // }

  bool HasAny() { return ops.size() > 0; }
  llvm::ArrayRef<TypeOperator> GetTypeOperators() { return ops; }

  // void Apply();
  // void Verify();
};

} // namespace syn
} // namespace stone
#endif
