#ifndef STONE_AST_TYPES_H
#define STONE_AST_TYPES_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/InlineBitfield.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/Type.h"
#include "stone/AST/TypeKind.h"
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
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
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
#include <string>
#include <type_traits>
#include <utility>

#include <string>

namespace stone {

class Type;
class TypeBase;
class CanType;
class TypeCollector;
class TypeWalker;
class SweetType;
class ASTContext;
class ASTPrinter;
class EnumDecl;
class ModuleDecl;
class InterfaceType;
class StructDecl;

class alignas(1 << TypeAlignInBits) TypeBase
    : public ASTAllocation<std::aligned_storage<8, 8>::type> {

  friend ASTContext;
  friend TypeCollector;

  TypeBase(const TypeBase &) = delete;
  void operator=(const TypeBase &) = delete;

  TypeKind kind;

  /// canType - This field is always set to the ASTContext for canonical
  /// types, and is otherwise lazily populated by ASTContext when the canonical
  /// form of a non-canonical type is requested.
  llvm::PointerUnion<TypeBase *, const ASTContext *> canonicalType;

protected:
  union {
    uint64_t OpaqueBits;

    STONE_INLINE_BITFIELD_BASE(TypeBase,
                               stone::BitMax(NumTypeKindBits, 8) +
                                   TypeQualifiers::BitWidth + 1 + 1,
                               /// Kind - The discriminator that indicates what
                               /// subclass of type this is.
                               kind
                               : stone::BitMax(NumTypeKindBits, 8),

                                 qualifiers
                               : TypeQualifiers::BitWidth,

                                 /// Whether this type is canonical or not.
                                 IsCanonical : 1,

                                 AllowQualifiers : 1);

    STONE_INLINE_BITFIELD(SweetType, TypeBase, 1, HasCachedType : 1);

  } Bits;

public:
  TypeBase(TypeKind kind, const ASTContext *canType, TypeQualifiers qualifiers)
      : kind(kind), canonicalType((TypeBase *)nullptr) {
    Bits.TypeBase.kind = static_cast<unsigned>(kind);
    if (canType) {
      canonicalType = canType;
    }
    SetQualifiers(qualifiers);
  }

  void SetQualifiers(TypeQualifiers qualifiers) {
    Bits.TypeBase.qualifiers = qualifiers.GetBits();
    assert(Bits.TypeBase.qualifiers == qualifiers.GetBits() && "Bits dropped!");
  }

  /// getRecursiveProperties - Returns the properties defined on the
  /// structure of this type.
  TypeQualifiers GetQualifiers() const {
    return TypeQualifiers(Bits.TypeBase.qualifiers);
  }

public:
  bool IsBasic();
  bool IsNominalType();
  bool IsBuiltinType() const;
  bool IsFunType() const;
  bool IsStructType() const;
  bool IsPointerType() const;
  bool IsReferenceType() const;

  /// getASTContext - Return the ASTContext that this type belongs to.
  ASTContext &GetASTContext();

public:
  TypeKind GetKind() const { return kind; }

  // TypeKind GetKind() const { return
  // static_cast<TypeKind>(Bits.Type.Kind); }

public:
  // Qualifiers
  bool AllowQualifiers() const { return Bits.TypeBase.AllowQualifiers; }

  bool HasConstQualifer() const { return GetQualifiers().HasConst(); }

  bool HasPermQualifer() const { return GetQualifiers().HasPerm(); }

  bool HasOwnQualifer() const { return GetQualifiers().HasOwn(); }

  bool HasMutableQualifer() const { return GetQualifiers().HasMutable(); }

public:
  /// isCanonical - Return true if this is a canonical type.
  bool IsCanonical() const { return Bits.TypeBase.IsCanonical; }
  /// isCanonical - Return true if this is a canonical type.
  bool IsCanType() const { return canonicalType.is<const ASTContext *>(); }

  /// hasCanonicalTypeComputed - Return true if we've already computed a
  /// canonical version of this type.
  bool IsCanTypeComputed() const { return !canonicalType.isNull(); }

  // CanType GetCanType() const {
  //   if (IsCanonical()) {
  //     return CanType(const_cast<TypeBase *>(this));
  //   }
  //   if (IsCanTypeComputed()) {
  //     return canonicalType;
  //   }
  //   return const_cast<TypeBase *>(this)->ComputeCanType();
  // }

private:
  CanType ComputeCanType();

public:
};

// TODO: Think about
//  class AnyType : public Type {
//  public:

//   AnyType(TypeKind kind, ASTContext *canType)
//       : Type(kind, canType) {}
// };

class FunctionType : public TypeBase {
  Type result;

public:
  FunctionType(TypeKind kind, Type result, const ASTContext *canType,
               TypeQualifiers qualifiers)
      : TypeBase(kind, canType, qualifiers) {}
};

// You are returning Type for now, it may have to be Type
class FunType : public FunctionType,
                private llvm::TrailingObjects<FunType, Type> {
  friend TrailingObjects;

public:
  FunType(Type result, const ASTContext *astContext, TypeQualifiers qualifiers);

public:
  static FunType *Create(Type result, const ASTContext &astContext,
                         TypeQualifiers qualifiers = TypeQualifiers());
};

class NominalType : public TypeBase {
protected:
  friend ASTContext;

public:
  // Implement isa/cast/dyncast/etc.
  static bool classof(const Type *ty) {
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
  BuiltinType(TypeKind kind, const ASTContext &astContext,
              TypeQualifiers qualifiers)
      : TypeBase(kind, &astContext, qualifiers) {}
};

class IdentifierType : public Type {};

class ScalarType : public BuiltinType {
public:
  ScalarType(TypeKind kind, const ASTContext &astContext,
             TypeQualifiers qualifiers)
      : BuiltinType(kind, astContext, qualifiers) {}
};

class CharType : public ScalarType {
public:
  CharType(const ASTContext &astContext, TypeQualifiers qualifiers)
      : ScalarType(TypeKind::Char, astContext, qualifiers) {}
};

class BoolType : public ScalarType {
public:
  BoolType(const ASTContext &astContext, TypeQualifiers qualifiers)
      : ScalarType(TypeKind::Bool, astContext, qualifiers) {}

public:
  static BoolType *Create(const ASTContext &astContext, AllocationArena aread,
                          TypeQualifiers qualifiers = TypeQualifiers());
};

// class StringType : public BuiltinType {
// public:
//   StringType(const ASTContext &astContext) : ScalarType(TypeKind::String,
//   astContext) {}
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
             const ASTContext &astContext, TypeQualifiers qualifiers)
      : ScalarType(kind, astContext, qualifiers), bitWidthKind(bitWidthKind) {}

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
  IntegerType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext,
              TypeQualifiers qualifiers)
      : NumberType(TypeKind::Integer, bitWidthKind, astContext,qualifiers) {}

public:
  static IntegerType *Create(NumberBitWidthKind bitWidthKind,
                             const ASTContext &astContext,
                             AllocationArena aread, TypeQualifiers qualifiers);
};

class UIntegerType : public NumberType {
  friend class ASTContext;

public:
  UIntegerType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext,
               TypeQualifiers qualifiers)

      : NumberType(TypeKind::UInteger, bitWidthKind, astContext, qualifiers) {}

public:
  static UIntegerType *Create(NumberBitWidthKind bitWidthKind, const ASTContext &astContext,
                              AllocationArena aread, TypeQualifiers qualifiers);
};

class ComplexType : public NumberType {
  friend class ASTContext;

public:
  ComplexType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext,
              TypeQualifiers qualifiers)
      : NumberType(TypeKind::Complex, bitWidthKind, astContext, qualifiers) {}
};

class ImaginaryType : public NumberType {
  friend class ASTContext;

public:
  ImaginaryType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext,
                TypeQualifiers qualifiers)
      : NumberType(TypeKind::Imaginary, bitWidthKind, astContext, qualifiers) {}

public:
  static ImaginaryType *Create(NumberBitWidthKind bitWidthKind,
                               const ASTContext &astContext,
                               AllocationArena aread,
                               TypeQualifiers qualifiers);
};

class FloatType : public NumberType {
  friend class ASTContext;

public:
  FloatType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext,
            TypeQualifiers qualifiers)
      : NumberType(TypeKind::Float, bitWidthKind, astContext, qualifiers) {}

public:
  static FloatType *Create(NumberBitWidthKind bitWidthKind,
                           const ASTContext &astContext, AllocationArena aread,
                           TypeQualifiers qualifiers);

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;
  static bool classof(const Type *T) { return T->GetKind() == TypeKind::Float; }
};

class VoidType : public BuiltinType {
public:
  VoidType(const ASTContext &astContext, TypeQualifiers qualifiers)
      : BuiltinType(TypeKind::Void, astContext, qualifiers) {}

public:
  static VoidType *Create(const ASTContext &astContext, AllocationArena arena,
                          TypeQualifiers qualifiers);
};

class NullType : public BuiltinType {
public:
  NullType(const ASTContext &astContext)
      : BuiltinType(TypeKind::Null, astContext, TypeQualifiers()) {}

public:
  static NullType *Create(const ASTContext &astContext, AllocationArena aread);
};

// This is how you deal with chunks
class SlabType : public TypeBase, public llvm::FoldingSetNode {
public:
};

class AbstractPointerType : public SlabType {
public:
  AbstractPointerType(TypeKind kind, const ASTContext &astContext,
                      TypeQualifiers qualifiers)
      : TypeBase(kind, &astContext, qualifiers) {}
};

class PointerType : public AbstractPointerType {
public:
};

class MemberPointerType : public AbstractPointerType {
public:
};

class ReferenceType : public TypeBase, public llvm::FoldingSetNode {};

class LValueReferenceType final : public ReferenceType {};

class RValueReferenceType final : public ReferenceType {};

class ModuleType : public TypeBase {
  ModuleDecl *const mod;

public:
  /// get - Return the ModuleType for the specified module.
  static ModuleType *Get(ModuleDecl *mod);
  ModuleDecl *GetModule() const { return mod; }

  // Implement isa/cast/dyncast/etc.
  static bool classof(const Type *ty) {
    return ty->GetKind() == TypeKind::Module;
  }

private:
  ModuleType(ModuleDecl *mod, const ASTContext &astContext)
      : TypeBase(TypeKind::Module, &astContext, TypeQualifiers()), mod(mod) {
    Bits.TypeBase.AllowQualifiers = false;
  }
};

class SweetType : public TypeBase {
  // The state of this union is known via Bits.SweetType.HasCachedType so that
  // we can avoid masking the pointer on the fast path.
  union {
    Type *underlyingType;
    const ASTContext *Context;
  };

  // llvm::PointerUnion<
};
/// An alias to a type
/// alias Int = int; My using use using Int = int;
class AliasType : public SweetType {
public:
};

/// A type with a special asttax that is always sugar for a library type. The
/// library type may have multiple base types. For unary asttax sugar, see
/// UnarySyntaxSweetType.
///
/// The prime examples are:
/// Arrays: [T] -> Array<T>
/// Dictionaries: [K : V]  -> Dictionary<K, V>
class SyntaxSweetType : public SweetType {
public:
};

/// The dictionary type [K : V], which is asttactic sugar for Dictionary<K, V>.
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
