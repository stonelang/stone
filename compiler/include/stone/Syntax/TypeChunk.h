#ifndef STONE_SYNTAX_TYPECHUNKH
#define STONE_SYNTAX_TYPECHUNKH

#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/TrailingObjects.h"

namespace stone {
namespace syn {

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
class alignas(1 << TypeAlignInBits) TypeChunk
    : syn::SyntaxAllocation<TypeChunk> {
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
  TypeChunkList(llvm::ArrayRef<TypeChunk> chunks);

public:
  static TypeChunkList *Create(llvm::ArrayRef<TypeChunk> chunks,
                               SyntaxContext &sc);
};

class TypeChunkCollector final {

  /// This holds each type-patter that the type-specifer includes as it is
  /// parsed.  This is pushed from the type out, which means that element
  /// #0 will be the most closely bound to the type, and
  /// chunks.back() will be the least closely bound to the type.
  llvm::SmallVector<TypeChunk, 8> chunks;

  /// If this Declarator declares a template, its template parameter lists.
  // llvm::ArrayRef<TemplateParameterList *> templateParameterLists;
public:
  TypeChunkCollector() {}

private:
  /// Add a chunk to this Declarator. Also extend the range to
  /// EndLoc, which should be the last token of the chunk.
  void AddTypeChunk(const TypeChunk chunk) {
    chunks.push_back(chunk);
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
  const TypeChunk *GetInnermostNonParenChunk() const {
    for (unsigned i = chunks.size(), i_end = 0; i != i_end; --i) {
      if (chunks[i - 1].GetKind() != TypeChunkKind::Paren) {
        return &chunks[i - 1];
      }
    }
    return nullptr;
  }

  /// int** -- the '*' farthest from int
  const TypeChunk *GetOutermostNonParenChunk() const {
    for (unsigned i = 0, i_end = chunks.size(); i < i_end; ++i) {
      if (chunks[i].GetKind() != TypeChunkKind::Paren) {
        return &chunks[i];
      }
    }
    return nullptr;
  }

  bool HasAny() { return chunks.size() > 0; }
  llvm::ArrayRef<TypeChunk> GetTypeChunks() { return chunks; }
  void Apply();
  void Verify();
};

} // namespace syn
} // namespace stone
#endif
