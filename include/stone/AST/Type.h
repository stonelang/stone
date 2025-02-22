#ifndef STONE_AST_TYPE_H
#define STONE_AST_TYPE_H

#include "stone/AST/Modifier.h"
#include "stone/AST/QualSpecs.h"
#include "stone/AST/TypeMetadata.h"
#include "stone/Basic/Basic.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/type_traits.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {

class ASTPrinter;
class EnumDecl;
class ModuleDecl;
class InterfaceType;
class StructDecl;
class UnderlyingType;
class Type;
class TypeWalker;

enum class GCKind : uint8 { None = 0, Weak, Strong };

/// ref-qualifier associated with a function Type.
/// This determines whether a member function's "this" object can be an
/// lvalue, rvalue, or neither.
enum class RefQualifierKind : uint8 {
  /// No ref-qualifier was provided.
  None = 0,
  /// An lvalue ref-qualifier was provided (\c &).
  LValue,

  /// An rvalue ref-qualifier was provided (\c &&).
  RValue
};

class Type {
  UnderlyingType *typePtr;
  // TypeModifierList Modifiers;
  //  TypeMetadata* metadata;
public:
  Type(UnderlyingType *typePtr = 0) : typePtr(typePtr) {}

public:
  bool IsNull() const { return typePtr == 0; }
  UnderlyingType *GetPtr() const { return typePtr; }
  UnderlyingType *operator->() const {
    assert(typePtr && "Cannot dereference a null Type!");
    return typePtr;
  }
  explicit operator bool() const { return typePtr != 0; }

  // const TypeModifierList &GetModifiers() const { return Modifiers; }
  // TypeModifierList &GetModifiers() { return Modifiers; }

public:
  /// Walk this Type.
  ///
  /// Returns true if the walk was aborted.
  bool Walk(TypeWalker &walker) const;
  bool Walk(TypeWalker &&walker) const { return Walk(walker); }

public:
  // void SetTypeChunkList(TypeChunkList *inputChunks) { thunks = inputChunks; }
  // TypeChunkList *GetTypeChunkList() { return thunks; }

  // static const TypeMetadata &Reflect(const Type &type)l

private:
  // Direct comparison is disabled for types, because they may not be canonical.
  void operator==(Type T) const = delete;
  void operator!=(Type T) const = delete;
};

class CanType : public Type {
public:
  /// Constructs a NULL canonical type.
  CanType() = default;

public:
  explicit CanType(UnderlyingType *ty) : Type(ty) {
    // TODO: assert(IsCanTypeOrNull() &&
    //        "Forming a CanType out of a non-canonical type!");
  }
  explicit CanType(Type ty) : Type(ty) {
    // TODO: assert(IsCanTypeOrNull() &&
    //        "Forming a CanType out of a non-canonical type!");
  }

private:
  bool IsCanTypeOrNull() const;

public:
  // Direct comparison is allowed for CanTypes - they are known canonical.
  bool operator==(CanType T) const { return GetPtr() == T.GetPtr(); }
  bool operator!=(CanType T) const { return !operator==(T); }
  bool operator<(CanType T) const { return GetPtr() < T.GetPtr(); }
};

} // namespace stone
#endif