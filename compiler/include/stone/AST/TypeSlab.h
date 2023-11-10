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

// class alignas(1 << TypeAlignInBits) TypeSlabs final {
//   /// This holds each type-slab that the type-specifer includes as it is
//   /// parsed.  This is pushed from the type out, which means that element
//   /// #0 will be the most closely bound to the type, and
//   /// slabs.back() will be the least closely bound to the type.
//   llvm::SmallVector<TypeSlabKind::Value, 8> slabs;

// private:
//   bool FindSlab(TypeSlabKind kind){
//     bool found = false;
//     for(auto slab : slabs){
//       if(slab == kind){
//         found = true;
//         break;
//       }
//     }
//     return found;
//   }
// public:
//   TypeSlabs(){}
// public:
//   void AddValue() {
//       slabs.push_back(TypeSlabKind::Value))
//   }
//   bool HasValue() {
//     return FindSlab(TypeSlabKind::Value);
//   }
// public:

//   void AddPointer() {
//       slabs.push_back(TypeSlabKind::Pointer))
//   }
//   void AddReference() {
//       slabs.push_back(TypeSlabKind::Reference))
//   }

// public:
//   /// int** -- the '*' closest to int
//   const TypeSlabKind GetInnermostNonParenSlab() const {
//       for (unsigned i = slabs.size(), i_end = 0; i != i_end; --i) {
//         if (slabs[i - 1].GetKind() != TypeSlabKind::Paren) {
//           return slabs[i - 1];
//         }
//       }
//       return TypeSlab::None;
//   }

//   /// int** -- the '*' farthest from int
//   const TypeSlabKind GetOutermostNonParenSlab() const {
//       for (unsigned i = 0, i_end = slabs.size(); i < i_end; ++i) {
//         if (slabs[i].GetKind() != TypeSlabKind::Paren) {
//           return slabs[i];
//         }
//       }
//       return TypeSlab::None;
//   }
// }

class alignas(1 << TypeAlignInBits) TypeSlab {
  SrcLoc loc;
  TypeSlabKind kind;

public:
  TypeSlab(TypeSlabKind kind, SrcLoc inputLoc) : kind(kind), loc(inputLoc) {}

public:
  TypeSlabKind GetKind() const { return kind; }
  SrcLoc GetLoc() { return loc; }
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
  void AddTypeSlab(const TypeSlab slab) { slabs.push_back(slab); }

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
  void Apply();
  void Verify();
};

} // namespace stone
#endif
