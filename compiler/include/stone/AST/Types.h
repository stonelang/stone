#ifndef STONE_SYNTAX_TYPES_H
#define STONE_SYNTAX_TYPES_H

#include "stone/Basic/Memory.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/AST/Foreign.h"
#include "stone/AST/InlineBitfield.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/Type.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/TypeChunk.h"
#include "stone/AST/TypeKind.h"
#include "stone/AST/TypeQualifier.h"

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
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
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

#include <string>

namespace stone {

class Type;
class TypeWalker;
class CanType;
class SweetType;

class alignas(1 << TypeAlignInBits) TypeBase
    : public MemoryAllocation<std::aligned_storage<8, 8>::type> {

  friend class ASTContext;

  TypeBase(const TypeBase &) = delete;
  void operator=(const TypeBase &) = delete;

  TypeKind kind;

  /// This union contains to the ASTContext for canonical types, and is
  /// otherwise lazily populated by ASTContext when the canonical form of a
  /// non-canonical type is requested. The disposition of the union is stored
  /// outside of the union for performance. See Bits.Type.IsCanonical.
  union {
    CanType canType;
    const ASTContext *astContext;
  };

protected:
  union {
    uint64_t OpaqueBits;
    STONE_INLINE_BITFIELD_BASE(TypeBase,
                               stone::BitMax(NumTypeKindBits, 8) + 1 + 1 + 1,
                               Kind
                               : stone::BitMax(NumTypeKindBits, 8),

                                 /// Whether this type is canonical or not.
                                 IsCanonical : 1,
                                 // Whether this type can have qualifiers
                                 AllowQuals : 1,

                                 IsBuiltin : 1);

    STONE_INLINE_BITFIELD(SweetType, TypeBase, 1, HasCachedType : 1);

  } Bits;

public:
  TypeBase(TypeKind kind, const ASTContext *canTypeContext)
      : kind(kind), astContext(nullptr) {

    Bits.TypeBase.Kind = static_cast<unsigned>(kind);

    /// TODO: I do not like this ....
    if (canTypeContext) {
      // Bits.Type.IsCanonical = true;
      astContext = canTypeContext;
    }
  }

public:
  bool IsBasic();
  bool IsNominalType();

  /// getASTContext - Return the ASTContext that this type belongs to.
  ASTContext &GetASTContext();

public:
  TypeKind GetKind() const { return kind; }

  // TypeKind GetKind() const { return
  // static_cast<TypeKind>(Bits.Type.Kind); }

  // We can do this because all types are generally cannonical types.
  // CanType GetCanType();

  /// isCanonical - Return true if this is a canonical type.
  bool IsCanType() const { return Bits.TypeBase.IsCanonical; }

  bool AllowQuals() const { return Bits.TypeBase.AllowQuals; }

  bool HasQuals() const;

  /// hasCanonicalTypeComputed - Return true if we've already computed a
  /// canonical version of this type.
  bool IsCanTypeComputed() const { return !canType.IsNull(); }

private:
  CanType ComputeCanType();
};

inline bool CanType::IsCanTypeOrNull() const {
  return IsNull() ||
         GetPtr() == llvm::DenseMapInfo<TypeBase *>::getEmptyKey() ||
         GetPtr() == llvm::DenseMapInfo<TypeBase *>::getTombstoneKey() ||
         GetPtr()->IsCanType();
}

class SourType : public TypeBase {
public:
};

class FunctionType : public TypeBase {
  Type result;

public:
  FunctionType(TypeKind kind, Type result, const ASTContext *canTypeCtx)
      : TypeBase(kind, canTypeCtx) {}
};

// You are returning Type for now, it may have to be QualType
class FunType : public FunctionType,
                private llvm::TrailingObjects<FunType, Type> {
  friend TrailingObjects;

public:
  FunType(Type result, const ASTContext *sc);
};

class NominalType : public TypeBase {
protected:
  friend ASTContext;

public:
  // Implement isa/cast/dyncast/etc.
  static bool classof(const TypeBase *ty) {
    return ty->GetKind() >= TypeKind::First_NominalType &&
           ty->GetKind() <= TypeKind::Last_NominalType;
  }
};

class StructType final : public NominalType {
public:
};

class InterfaceType final : public NominalType {
public:
};

class EnumType final : public NominalType {};

class DeducedType : public TypeBase {
protected:
  friend class ASTContext; // ASTContext creates these
};

class AutoType final : public DeducedType, public llvm::FoldingSetNode {
public:
};

// class TemplateParmType : public Type{
// };

class BuiltinType : public TypeBase {
protected:
  BuiltinType(TypeKind kind, const ASTContext &sc) : TypeBase(kind, &sc) {
    // Bits.TypeBase.IsBuiltin = true;
  }
};

class IdentifierType : public TypeBase {};

class ScalarType : public BuiltinType {
public:
  ScalarType(TypeKind kind, const ASTContext &sc) : BuiltinType(kind, sc) {}
};

class CharType : public ScalarType {
public:
  CharType(const ASTContext &sc) : ScalarType(TypeKind::Char, sc) {}
};

class BoolType : public ScalarType {
public:
  BoolType(const ASTContext &sc) : ScalarType(TypeKind::Bool, sc) {}
};

// class StringType : public BuiltinType {
// public:
//   StringType(const ASTContext &sc) : ScalarType(TypeKind::String, sc) {}
// };

struct NumberBitWidth final {
  enum Kind : UInt8 {
    Platform = 0,
    N8,
    N16,
    N32,
    N64,
    N80,
    N128,
  };
  static unsigned GetNumberBitWidth(NumberBitWidth::Kind kind) {
    switch (kind) {
    case NumberBitWidth::Platform:
      return 0;
    case NumberBitWidth::N8:
      return 8;
    case NumberBitWidth::N16:
      return 16;
    case NumberBitWidth::N32:
      return 32;
    case NumberBitWidth::N64:
      return 64;
    case NumberBitWidth::N80:
      return 80;
    case NumberBitWidth::N128:
      return 128;
    }
    llvm_unreachable("Valid bit widths are: 8 | 16 | 32 | 64 | 80 | 128");
  }
};
using NumberBitWidthKind = NumberBitWidth::Kind;
/// An abstract base class for integers and floats
class NumberType : public ScalarType {
  NumberBitWidthKind bitWidthKind;

public:
  NumberType(TypeKind kind, NumberBitWidthKind bitWidthKind,
             const ASTContext &sc)
      : ScalarType(kind, sc), bitWidthKind(bitWidthKind) {}

public:
  unsigned GetNumberBitWidth() const {
    return NumberBitWidth::GetNumberBitWidth(bitWidthKind);
  }
  bool UsePlatformNumberBitWidth() {
    return GetNumberBitWidth() == NumberBitWidth::Platform;
  }
};

class IntegerType : public NumberType {
  friend class ASTContext;

public:
  IntegerType(NumberBitWidthKind bitWidthKind, const ASTContext &sc)
      : NumberType(TypeKind::Integer, bitWidthKind, sc) {}

public:
  static IntegerType *Create(NumberBitWidthKind bitWidthKind,
                             const ASTContext &sc, MemoryAllocationArena arena);
};

class UIntegerType : public NumberType {
  friend class ASTContext;

public:
  UIntegerType(NumberBitWidthKind bitWidthKind, const ASTContext &sc)

      : NumberType(TypeKind::UInteger, bitWidthKind, sc) {}
};

class ComplexType : public NumberType {
  friend class ASTContext;

public:
  ComplexType(NumberBitWidthKind bitWidthKind, const ASTContext &sc)
      : NumberType(TypeKind::Complex, bitWidthKind, sc) {}
};

class ImaginaryType : public NumberType {
  friend class ASTContext;

public:
  ImaginaryType(NumberBitWidthKind bitWidthKind, const ASTContext &sc)
      : NumberType(TypeKind::Imaginary, bitWidthKind, sc) {}
};

class FloatType : public NumberType {
  friend class ASTContext;

public:
  FloatType(NumberBitWidthKind bitWidthKind, const ASTContext &sc)
      : NumberType(TypeKind::Float, bitWidthKind, sc) {}

public:
  static FloatType *Create(NumberBitWidthKind bitWidthKind,
                           const ASTContext &astContext,
                           MemoryAllocationArena arena);

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;
  static bool classof(const TypeBase *T) {
    return T->GetKind() == TypeKind::Float;
  }
};

class AnyType : public BuiltinType {
public:
  AnyType(const ASTContext &astContext)
      : BuiltinType(TypeKind::Any, astContext) {}
};

class VoidType : public BuiltinType {
public:
  VoidType(const ASTContext &sc) : BuiltinType(TypeKind::Void, sc) {}

public:
  static VoidType *
  Create(const ASTContext &astContext,
         MemoryAllocationArena arena = MemoryAllocationArena::Permanent);
};

class NullType : public BuiltinType {
public:
  NullType(const ASTContext &astContext)
      : BuiltinType(TypeKind::Null, astContext) {}
};

class ChunkType : public TypeBase, public llvm::FoldingSetNode {};

class AbstractPointerType : public TypeBase, public llvm::FoldingSetNode {

  Type pointeeType;

public:
  AbstractPointerType(TypeKind kind, const ASTContext &astContext)
      : TypeBase(kind, &astContext) {}
};

class PointerType : public AbstractPointerType {

  // PointerType(Type pointeeType, Type canType)
  //     : AbstractPointerType(Pointer, CanonicalPtr, Pointee->getDependence()),
  //       PointeeType(Pointee) {}

public:
};

class MemberPointerType : public AbstractPointerType {

public:
};

class AbstractReferenceType : public TypeBase, public llvm::FoldingSetNode {
public:
};

class LValueReferenceType final : public AbstractReferenceType {
public:
};

class RValueReferenceType final : public AbstractReferenceType {
public:
};

class ModuleType : public TypeBase {
  ModuleDecl *const mod;

public:
  /// get - Return the ModuleType for the specified module.
  static ModuleType *Get(ModuleDecl *mod);
  ModuleDecl *GetModule() const { return mod; }

  // Implement isa/cast/dyncast/etc.
  static bool classof(const TypeBase *ty) {
    return ty->GetKind() == TypeKind::Module;
  }

private:
  ModuleType(ModuleDecl *mod, const ASTContext &sc)
      : TypeBase(TypeKind::Module, &sc), mod(mod) {}
};

class SweetType : public TypeBase {
  // The state of this union is known via Bits.SweetType.HasCachedType so that
  // we can avoid masking the pointer on the fast path.
  union {
    Type *underlyingType;
    const ASTContext *Context;
  };
};
/// An alias to a type
/// alias Int = int; My using use using Int = int;
class AliasType : public SweetType {
public:
};

/// An alias to a type
/// using Int = int; My using use using Int = int;
class UsingType : public SweetType {
public:
};

/// A type with a special syntax that is always sugar for a library type. The
/// library type may have multiple base types. For unary syntax sugar, see
/// UnarySyntaxSweetType.
///
/// The prime examples are:
/// Arrays: [T] -> Array<T>
/// Dictionaries: [K : V]  -> Dictionary<K, V>
class SyntaxSweetType : public SweetType {
public:
};

/// The dictionary type [K : V], which is syntactic sugar for Dictionary<K, V>.
///
/// Example:
/// \code
/// auto dict: [string : int] = ["hello" : 0, "world" : 1]
/// \endcode
class DictionaryType : public SyntaxSweetType {
public:
};

// class ArrayType : public Type, public llvm::FoldingSetNode {
// public:
// };

} // namespace stone
#endif