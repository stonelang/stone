#ifndef STONE_AST_TYPESLAB_H
#define STONE_AST_TYPESLAB_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Specifier.h"
#include "stone/AST/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {

enum class TypeSlabKind {
  None,
  Value,
  Pointer,
  Reference,
  Array,
  MemberPointer,
  Paren,
  Pipe,
};
class alignas(1 << TypeAlignInBits) TypeSlab
    : stone::ASTAllocation<TypeSlab> {
  SrcLoc loc;
  TypeSlabKind kind;

public:
  TypeSlab(TypeSlabKind kind, SrcLoc inputLoc) : kind(kind), loc(inputLoc) {}

public:
  TypeSlabKind GetKind() const { return kind; }
  SrcLoc GetLoc() { return loc; }
};

class ValueTypeSlab final : public TypeSlab {
public:
  ValueTypeSlab() : TypeSlab(TypeSlabKind::Value, SrcLoc()) {}

public:
  static ValueTypeSlab Create();
};

class PointerTypeSlab final : public TypeSlab {
public:
  PointerTypeSlab(SrcLoc loc) : TypeSlab(TypeSlabKind::Pointer, loc) {}

public:
  static PointerTypeSlab Create(SrcLoc loc);
};

class MemberPointerTypeSlab final : public TypeSlab {
public:
  MemberPointerTypeSlab(SrcLoc loc)
      : TypeSlab(TypeSlabKind::MemberPointer, loc) {}

public:
  static MemberPointerTypeSlab Create(SrcLoc loc);
};

class ReferenceTypeSlab final : public TypeSlab {
public:
  ReferenceTypeSlab(SrcLoc loc) : TypeSlab(TypeSlabKind::Reference, loc) {}

public:
  static ReferenceTypeSlab Create(SrcLoc loc);
};

class ArrayTypeSlab final : public TypeSlab {
public:
  ArrayTypeSlab(SrcLoc loc) : TypeSlab(TypeSlabKind::Array, loc) {}

public:
  static ArrayTypeSlab Create(SrcLoc loc);
};

class ParenTypeSlab final : public TypeSlab {
public:
  ParenTypeSlab(SrcLoc loc) : TypeSlab(TypeSlabKind::Paren, loc) {}

public:
  static ParenTypeSlab Create(SrcLoc loc);
};

// class FunctionTypeSlab final : public TypeSlab {
// public:
//   FunctionTypeSlab() :
//   TypeSlab(TypeSlabKind::Function) {}

// public:
//   static FunctionTypeSlab Create();
// };

class TypeSlabList final
    : private llvm::TrailingObjects<TypeSlabList, TypeSlab> {

  friend TrailingObjects;

public:
  /// No copying
  TypeSlabList(const TypeSlabList &) = delete;
  TypeSlabList &operator=(const TypeSlabList &) = delete;

public:
  TypeSlabList(llvm::ArrayRef<TypeSlab> slabs);

public:
  static TypeSlabList *Create(llvm::ArrayRef<TypeSlab> slabs,
                               ASTContext &sc);
};

class TypeSlabCollector final {

  /// This holds each type-patter that the type-specifer includes as it is
  /// parsed.  This is pushed from the type out, which means that element
  /// #0 will be the most closely bound to the type, and
  /// slabs.back() will be the least closely bound to the type.
  llvm::SmallVector<TypeSlab, 8> slabs;

  /// If this Declarator declares a template, its template parameter lists.
  // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;
public:
  TypeSlabCollector() {}

private:
  /// Add a thunk to this Declarator. Also extend the range to
  /// EndLoc, which should be the last token of the thunk.
  void AddTypeSlab(const TypeSlab thunk) {
    slabs.push_back(thunk);
    // TODO:
    //  if (!EndLoc.isInvalid())
    //    SetRangeEnd(EndLoc);
  }

public:
  // Value has no source loc
  void AddValue();
  void AddPointer(SrcLoc loc);
  void AddMemberPointer(SrcLoc loc);
  void AddReference(SrcLoc loc);
  void AddArray(SrcLoc loc);
  void AddParen(SrcLoc loc);
  void AddPipe(SrcLoc loc);

public:
  /// int** -- the '*' toucing int
  const TypeSlab *GetInnermostNonParenSlab() const {
    for (unsigned i = slabs.size(), i_end = 0; i != i_end; --i) {
      if (slabs[i - 1].GetKind() != TypeSlabKind::Paren) {
        return &slabs[i - 1];
      }
    }
    return nullptr;
  }

  /// int** -- the '*' farthest from int
  const TypeSlab *GetOutermostNonParenSlab() const {
    for (unsigned i = 0, i_end = slabs.size(); i < i_end; ++i) {
      if (slabs[i].GetKind() != TypeSlabKind::Paren) {
        return &slabs[i];
      }
    }
    return nullptr;
  }

  bool HasAny() { return slabs.size() > 0; }
  llvm::ArrayRef<TypeSlab> GetTypeSlabs() { return slabs; }
  void Apply();
  void Verify();
};

} // namespace stone
#endif
