#ifndef STONE_SYNTAX_TYPETHUNK_H
#define STONE_SYNTAX_TYPETHUNK_H

#include "stone/Syntax/ASTAllocation.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
namespace syn {

enum class TypeThunkKind {
  None,
  Value,
  Pointer,
  Reference,
  Array,
  MemberPointer,
  Paren,
  Pipe,
};
class alignas(1 << TypeAlignInBits) TypeThunk : syn::ASTAllocation<TypeThunk> {
  SrcLoc loc;
  TypeThunkKind kind;

public:
  TypeThunk(TypeThunkKind kind, SrcLoc inputLoc) : kind(kind), loc(inputLoc) {}

public:
  TypeThunkKind GetKind() const { return kind; }
  SrcLoc GetLoc() { return loc; }
};

class ValueTypeThunk final : public TypeThunk {
public:
  ValueTypeThunk() : TypeThunk(TypeThunkKind::Value, SrcLoc()) {}

public:
  static ValueTypeThunk Create();
};

class PointerTypeThunk final : public TypeThunk {
public:
  PointerTypeThunk(SrcLoc loc) : TypeThunk(TypeThunkKind::Pointer, loc) {}

public:
  static PointerTypeThunk Create(SrcLoc loc);
};

class MemberPointerTypeThunk final : public TypeThunk {
public:
  MemberPointerTypeThunk(SrcLoc loc)
      : TypeThunk(TypeThunkKind::MemberPointer, loc) {}

public:
  static MemberPointerTypeThunk Create(SrcLoc loc);
};

class ReferenceTypeThunk final : public TypeThunk {
public:
  ReferenceTypeThunk(SrcLoc loc) : TypeThunk(TypeThunkKind::Reference, loc) {}

public:
  static ReferenceTypeThunk Create(SrcLoc loc);
};

class ArrayTypeThunk final : public TypeThunk {
public:
  ArrayTypeThunk(SrcLoc loc) : TypeThunk(TypeThunkKind::Array, loc) {}

public:
  static ArrayTypeThunk Create(SrcLoc loc);
};

class ParenTypeThunk final : public TypeThunk {
public:
  ParenTypeThunk(SrcLoc loc) : TypeThunk(TypeThunkKind::Paren, loc) {}

public:
  static ParenTypeThunk Create(SrcLoc loc);
};

// class FunctionTypeThunk final : public TypeThunk {
// public:
//   FunctionTypeThunk() :
//   TypeThunk(TypeThunkKind::Function) {}

// public:
//   static FunctionTypeThunk Create();
// };

class TypeThunkList final
    : private llvm::TrailingObjects<TypeThunkList, TypeThunk> {

  friend TrailingObjects;

public:
  /// No copying
  TypeThunkList(const TypeThunkList &) = delete;
  TypeThunkList &operator=(const TypeThunkList &) = delete;

public:
  TypeThunkList(llvm::ArrayRef<TypeThunk> thunks);

public:
  static TypeThunkList *Create(llvm::ArrayRef<TypeThunk> thunks,
                               ASTContext &sc);
};

class TypeThunkCollector final {

  /// This holds each type-patter that the type-specifer includes as it is
  /// parsed.  This is pushed from the type out, which means that element
  /// #0 will be the most closely bound to the type, and
  /// thunks.back() will be the least closely bound to the type.
  llvm::SmallVector<TypeThunk, 8> thunks;

  /// If this Declarator declares a template, its template parameter lists.
  // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;
public:
  TypeThunkCollector() {}

private:
  /// Add a thunk to this Declarator. Also extend the range to
  /// EndLoc, which should be the last token of the thunk.
  void AddTypeThunk(const TypeThunk thunk) {
    thunks.push_back(thunk);
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
  const TypeThunk *GetInnermostNonParenThunk() const {
    for (unsigned i = thunks.size(), i_end = 0; i != i_end; --i) {
      if (thunks[i - 1].GetKind() != TypeThunkKind::Paren) {
        return &thunks[i - 1];
      }
    }
    return nullptr;
  }

  /// int** -- the '*' farthest from int
  const TypeThunk *GetOutermostNonParenThunk() const {
    for (unsigned i = 0, i_end = thunks.size(); i < i_end; ++i) {
      if (thunks[i].GetKind() != TypeThunkKind::Paren) {
        return &thunks[i];
      }
    }
    return nullptr;
  }

  bool HasAny() { return thunks.size() > 0; }
  llvm::ArrayRef<TypeThunk> GetTypeThunks() { return thunks; }
  void Apply();
  void Verify();
};

} // namespace syn
} // namespace stone
#endif
