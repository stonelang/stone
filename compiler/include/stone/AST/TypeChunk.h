#ifndef STONE_ASTTYPETHUNK_H
#define STONE_ASTTYPETHUNK_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Specifier.h"
#include "stone/AST/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
namespace ast {

enum class TypeChunkKind {
  None,
  Value,
  Pointer,
  Reference,
  Array,
  MemberPointer,
  Paren,
  Pipe,
};
class alignas(1 << TypeAlignInBits) TypeChunk : ast::ASTAllocation<TypeChunk> {
  SrcLoc loc;
  TypeChunkKind kind;

public:
  TypeChunk(TypeChunkKind kind, SrcLoc inputLoc) : kind(kind), loc(inputLoc) {}

public:
  TypeChunkKind GetKind() const { return kind; }
  SrcLoc GetLoc() { return loc; }
};

class ValueTypeChunk final : public TypeChunk {
public:
  ValueTypeChunk() : TypeChunk(TypeChunkKind::Value, SrcLoc()) {}

public:
  static ValueTypeChunk Create();
};

class PointerTypeChunk final : public TypeChunk {
public:
  PointerTypeChunk(SrcLoc loc) : TypeChunk(TypeChunkKind::Pointer, loc) {}

public:
  static PointerTypeChunk Create(SrcLoc loc);
};

class MemberPointerTypeChunk final : public TypeChunk {
public:
  MemberPointerTypeChunk(SrcLoc loc)
      : TypeChunk(TypeChunkKind::MemberPointer, loc) {}

public:
  static MemberPointerTypeChunk Create(SrcLoc loc);
};

class ReferenceTypeChunk final : public TypeChunk {
public:
  ReferenceTypeChunk(SrcLoc loc) : TypeChunk(TypeChunkKind::Reference, loc) {}

public:
  static ReferenceTypeChunk Create(SrcLoc loc);
};

class ArrayTypeChunk final : public TypeChunk {
public:
  ArrayTypeChunk(SrcLoc loc) : TypeChunk(TypeChunkKind::Array, loc) {}

public:
  static ArrayTypeChunk Create(SrcLoc loc);
};

class ParenTypeChunk final : public TypeChunk {
public:
  ParenTypeChunk(SrcLoc loc) : TypeChunk(TypeChunkKind::Paren, loc) {}

public:
  static ParenTypeChunk Create(SrcLoc loc);
};

// class FunctionTypeChunk final : public TypeChunk {
// public:
//   FunctionTypeChunk() :
//   TypeChunk(TypeChunkKind::Function) {}

// public:
//   static FunctionTypeChunk Create();
// };

class TypeChunkList final
    : private llvm::TrailingObjects<TypeChunkList, TypeChunk> {

  friend TrailingObjects;

public:
  /// No copying
  TypeChunkList(const TypeChunkList &) = delete;
  TypeChunkList &operator=(const TypeChunkList &) = delete;

public:
  TypeChunkList(llvm::ArrayRef<TypeChunk> thunks);

public:
  static TypeChunkList *Create(llvm::ArrayRef<TypeChunk> thunks,
                               ASTContext &sc);
};

class TypeChunkCollector final {

  /// This holds each type-patter that the type-specifer includes as it is
  /// parsed.  This is pushed from the type out, which means that element
  /// #0 will be the most closely bound to the type, and
  /// thunks.back() will be the least closely bound to the type.
  llvm::SmallVector<TypeChunk, 8> thunks;

  /// If this Declarator declares a template, its template parameter lists.
  // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;
public:
  TypeChunkCollector() {}

private:
  /// Add a thunk to this Declarator. Also extend the range to
  /// EndLoc, which should be the last token of the thunk.
  void AddTypeChunk(const TypeChunk thunk) {
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
  const TypeChunk *GetInnermostNonParenThunk() const {
    for (unsigned i = thunks.size(), i_end = 0; i != i_end; --i) {
      if (thunks[i - 1].GetKind() != TypeChunkKind::Paren) {
        return &thunks[i - 1];
      }
    }
    return nullptr;
  }

  /// int** -- the '*' farthest from int
  const TypeChunk *GetOutermostNonParenThunk() const {
    for (unsigned i = 0, i_end = thunks.size(); i < i_end; ++i) {
      if (thunks[i].GetKind() != TypeChunkKind::Paren) {
        return &thunks[i];
      }
    }
    return nullptr;
  }

  bool HasAny() { return thunks.size() > 0; }
  llvm::ArrayRef<TypeChunk> GetTypeChunks() { return thunks; }
  void Apply();
  void Verify();
};

} // namespace ast
} // namespace stone
#endif
