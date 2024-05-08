#ifndef STONE_AST_TYPES_H
#define STONE_AST_TYPES_H

#include "stone/AST/Foreign.h"
#include "stone/AST/InlineBitfield.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/Type.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/TypeChunk.h"
#include "stone/AST/TypeKind.h"
#include "stone/AST/TypeQualifier.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/STDAlias.h"
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

class BuiltinType : public TypeBase {
protected:
  BuiltinType(TypeKind kind, const ASTContext &sc) : TypeBase(kind, &sc) {
    // Bits.TypeBase.IsBuiltin = true;
  }
};

class IdentifierType : public TypeBase {
public:
};

// class StringType : public BuiltinType {
// public:
//   StringType(const ASTContext &sc) : ScalarType(TypeKind::String, sc) {}
// };

class NumericType : public BuiltinType {
protected:
  NumericType(TypeKind kind, const ASTContext &sc) : BuiltinType(kind, sc) {}
};

class SignedType : public NumericType {
protected:
  SignedType(TypeKind kind, const ASTContext &sc) : NumericType(kind, sc) {}
};

class IntType : public SignedType {
  friend class ASTContext;

public:
  IntType(const ASTContext &sc) : SignedType(TypeKind::Int, sc) {}

public:
  static IntType *Create(const ASTContext &sc);
};

class Int8Type : public SignedType {
  friend class ASTContext;

public:
  Int8Type(const ASTContext &sc) : SignedType(TypeKind::Int8, sc) {}

public:
  static Int8Type *Create(const ASTContext &sc);
};

class Int16Type : public SignedType {
  friend class ASTContext;

public:
  Int16Type(const ASTContext &sc) : SignedType(TypeKind::Int16, sc) {}

public:
  static Int16Type *Create(const ASTContext &sc);
};
class Int32Type : public SignedType {
  friend class ASTContext;

public:
  Int32Type(const ASTContext &sc) : SignedType(TypeKind::Int32, sc) {}

public:
  static Int16Type *Create(const ASTContext &sc);
};

class Int64Type : public SignedType {
  friend class ASTContext;

public:
  Int64Type(const ASTContext &sc) : SignedType(TypeKind::Int64, sc) {}

public:
  static Int64Type *Create(const ASTContext &sc);
};

class Int128Type : public SignedType {
  friend class ASTContext;

public:
  Int128Type(const ASTContext &sc) : SignedType(TypeKind::Int128, sc) {}

public:
  static Int128Type *Create(const ASTContext &sc);
};

class UnsignedType : public NumericType {
protected:
  UnsignedType(TypeKind kind, const ASTContext &sc) : NumericType(kind, sc) {}
};

class UIntType : public UnsignedType {
  friend class ASTContext;

public:
  UIntType(const ASTContext &sc) : UnsignedType(TypeKind::UInt, sc) {}
};
class UInt8Type : public UnsignedType {
  friend class ASTContext;

public:
  UInt8Type(const ASTContext &sc) : UnsignedType(TypeKind::UInt8, sc) {}
};
class UInt16Type : public UnsignedType {
  friend class ASTContext;

public:
  UInt16Type(const ASTContext &sc) : UnsignedType(TypeKind::UInt16, sc) {}
};

class UInt32Type : public UnsignedType {
  friend class ASTContext;

public:
  UInt32Type(const ASTContext &sc) : UnsignedType(TypeKind::UInt32, sc) {}
};

class UInt64Type final : public UnsignedType {
  friend class ASTContext;

public:
  UInt64Type(const ASTContext &sc) : UnsignedType(TypeKind::UInt64, sc) {}
};
class UInt128Type final : public UnsignedType {
  friend class ASTContext;

public:
  UInt128Type(const ASTContext &sc) : UnsignedType(TypeKind::UInt128, sc) {}
};

class CharType final : public UnsignedType {
public:
  CharType(const ASTContext &sc) : UnsignedType(TypeKind::Char, sc) {}
};
class Char8Type final : public UnsignedType {
public:
  Char8Type(const ASTContext &sc) : UnsignedType(TypeKind::Char8, sc) {}
};
class Char16Type final : public UnsignedType {
public:
  Char16Type(const ASTContext &sc) : UnsignedType(TypeKind::Char16, sc) {}
};
class Char32Type final : public UnsignedType {
public:
  Char32Type(const ASTContext &sc) : UnsignedType(TypeKind::Char32, sc) {}
};

class BoolType final : public UnsignedType {
public:
  BoolType(const ASTContext &sc) : UnsignedType(TypeKind::Bool, sc) {}
};

class Complex32Type final : public NumericType {
  friend class ASTContext;

public:
  Complex32Type(const ASTContext &sc) : NumericType(TypeKind::Complex32, sc) {}
};

class Complex64Type final : public NumericType {
  friend class ASTContext;

public:
  Complex64Type(const ASTContext &sc) : NumericType(TypeKind::Complex64, sc) {}
};

class Imaginary32Type final : public NumericType {
  friend class ASTContext;

public:
  Imaginary32Type(const ASTContext &sc)
      : NumericType(TypeKind::Imaginary32, sc) {}
};

class Imaginary64Type final : public NumericType {
  friend class ASTContext;

public:
  Imaginary64Type(const ASTContext &sc)
      : NumericType(TypeKind::Imaginary64, sc) {}
};

class FloatType : public NumericType {
  friend class ASTContext;

public:
  FloatType(const ASTContext &sc) : NumericType(TypeKind::Float, sc) {}

public:
  static FloatType *Create(const ASTContext &astContext);

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;
  static bool classof(const TypeBase *T) {
    return T->GetKind() == TypeKind::Float;
  }
};

class Float16Type : public NumericType {
  friend class ASTContext;

public:
  Float16Type(const ASTContext &sc) : NumericType(TypeKind::Float16, sc) {}
};

class Float32Type : public NumericType {
  friend class ASTContext;

public:
  Float32Type(const ASTContext &sc) : NumericType(TypeKind::Float32, sc) {}
};

class Float64Type : public NumericType {
  friend class ASTContext;

public:
  Float64Type(const ASTContext &sc) : NumericType(TypeKind::Float64, sc) {}
};
class Float128Type : public NumericType {
  friend class ASTContext;

public:
  Float128Type(const ASTContext &sc) : NumericType(TypeKind::Float128, sc) {}
};

// class TemplateParmType : public Type{
// };

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