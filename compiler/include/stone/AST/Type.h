#ifndef STONE_AST_TYPE_H
#define STONE_AST_TYPE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Foreign.h"
#include "stone/AST/InlineBitfield.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/QualType.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/TypeChunk.h"
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
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
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

#include <string>

namespace stone {

class QualType;
class TypeWalker;
class CanType;
class SugarType;
class Int8Type;
class Int16Type;
class FunType;
class StructType;
class ASTContext;

class alignas(1 << TypeAlignInBits) Type
    : public ASTAllocation<std::aligned_storage<8, 8>::type> {

  friend class ASTContext;

  Type(const Type &) = delete;
  void operator=(const Type &) = delete;

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
    STONE_INLINE_BITFIELD_BASE(Type,
                               stone::BitMax(NumTypeKindBits, 8) + 1 + 1 + 1,
                               Kind
                               : stone::BitMax(NumTypeKindBits, 8),

                                 /// Whether this type is canonical or not.
                                 IsCanonical : 1,
                                 // Whether this type can have qualifiers
                                 AllowQualifiers : 1,

                                 IsBuiltin : 1);

  } Bits;

public:
  Type(TypeKind kind, const ASTContext *canTypeContext) : astContext(nullptr) {

    Bits.Type.Kind = static_cast<unsigned>(kind);

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
  // We can do this because all types are generally cannonical types.
  // CanType GetCanType();

  /// isCanonical - Return true if this is a canonical type.
  bool IsCanType() const { return Bits.Type.IsCanonical; }

  bool AllowQualifiers() const { return Bits.Type.AllowQualifiers; }

  bool HasQualifiers() const;

  /// hasCanonicalTypeComputed - Return true if we've already computed a
  /// canonical version of this type.
  bool IsCanTypeComputed() const { return !canType.IsNull(); }

public:
  TypeKind GetKind() const { return static_cast<TypeKind>(Bits.Type.Kind); }

public:
  ///\return true if the type is a builtin type.
  bool IsBuiltinType() const;

  ///\return true if the type is a integer type : int, char, bool
  bool IsIntegerType() const;
  ///\return true if this is an integer type that is
  bool IsSignedIntegerType() const;
  ///\return true if the type is unsigned
  bool IsUnsignedIntegerType() const;

  ///\return true if the type is int8
  bool IsInt8Type() const;
  Int8Type *GetInt8Type() const;

  ///\return true if the type is int16
  bool IsInt16Type() const;
  Int16Type *GetInt16Type() const;

  ///\return true if the type is int32
  bool IsInt32Type() const;

  ///\return true if the type is int64
  bool IsInt64Type() const;
  ///\return true if the type is int128
  bool IsInt128Type() const;

  ///\return true if the type is uint8
  bool IsUIntType() const;
  ///\return true if the type is uint16
  bool IsUInt8Type() const;
  ///\return true if the type is uint16
  bool IsUInt16Type() const;
  ///\return true if the type is uint32
  bool IsUInt32Type() const;
  ///\return true if the type is uint64
  bool IsUInt64Type() const;
  ///\return true if the type is uint128
  bool IsUInt128Type() const;

  ///\return true if the type is bool
  bool IsBoolType() const;

  ///\return true if the type is char
  bool IsCharType() const;
  ///\return true if the type is char8
  bool IsChar8Type() const;
  ///\return true if the type is char16
  bool IsChar16Type() const;
  ///\return true if the type is char32
  bool IsChar32Type() const;

  ///\return true if the type is real
  bool IsRealType() const;

  ///\return true if the type is float
  bool IsFloatType() const;
  ///\return true if the type is float16
  bool IsFloat16Type() const;
  ///\return true if the type is float32
  bool IsFloat32Type() const;
  ///\return true if the type is float64
  bool IsFloat64Type() const;
  ///\return true if the type is float128
  bool IsFloat128Type() const;

  ///\return true if the type is complex32
  bool IsComplex32Type() const;
  ///\return true if the type is complex64
  bool IsComplex64Type() const;

  ///\return true if the type is imaginary32
  bool IsImaginary32Type() const;
  ///\return true if the type is imaginary32
  bool IsImaginary64Type() const;

  //\return true if the type is FunType
  bool IsFunType() const;
  FunType *GetFunType() const;

  bool IsStructType() const;
  StructType *GetStructType() const;

private:
  CanType ComputeCanType();
};

inline bool CanType::IsCanTypeOrNull() const {
  return IsNull() || GetPtr() == llvm::DenseMapInfo<Type *>::getEmptyKey() ||
         GetPtr() == llvm::DenseMapInfo<Type *>::getTombstoneKey() ||
         GetPtr()->IsCanType();
}

class BuiltinType : public Type {
protected:
  BuiltinType(TypeKind kind, const ASTContext &sc) : Type(kind, &sc) {
    // Bits.Type.IsBuiltin = true;
  }
};

// class IdentifierType : public Type {
// public:
// };

// // class StringType : public BuiltinType {
// // public:
// //   StringType(const ASTContext &sc) : BuiltinType(TypeKind::String, sc) {}
// // };

class VoidType : public BuiltinType {
public:
  VoidType(const ASTContext &astContext)
      : BuiltinType(TypeKind::Void, astContext) {}

public:
  static VoidType *Create(const ASTContext &astContext);
};

class NullType : public BuiltinType {
public:
  NullType(const ASTContext &astContext)
      : BuiltinType(TypeKind::Null, astContext) {}
};

class IntType : public BuiltinType {
  friend ASTContext;

public:
  IntType(const ASTContext &sc) : BuiltinType(TypeKind::Int, sc) {}

public:
  static IntType *Create(const ASTContext &sc);
};

class Int8Type : public BuiltinType {
  friend ASTContext;

public:
  Int8Type(const ASTContext &sc) : BuiltinType(TypeKind::Int8, sc) {}

public:
  static Int8Type *Create(const ASTContext &sc);
};

class Int16Type : public BuiltinType {
  friend ASTContext;

public:
  Int16Type(const ASTContext &sc) : BuiltinType(TypeKind::Int16, sc) {}

public:
  static Int16Type *Create(const ASTContext &sc);
};

class Int32Type : public BuiltinType {
  friend ASTContext;

public:
  Int32Type(const ASTContext &sc) : BuiltinType(TypeKind::Int32, sc) {}

public:
  static Int16Type *Create(const ASTContext &sc);
};

class Int64Type : public BuiltinType {
  friend ASTContext;

public:
  Int64Type(const ASTContext &sc) : BuiltinType(TypeKind::Int64, sc) {}

public:
  static Int64Type *Create(const ASTContext &sc);
};

class Int128Type : public BuiltinType {
  friend ASTContext;

public:
  Int128Type(const ASTContext &sc) : BuiltinType(TypeKind::Int128, sc) {}

public:
  static Int128Type *Create(const ASTContext &sc);
};

class UIntType : public BuiltinType {
  friend class ASTContext;

public:
  UIntType(const ASTContext &sc) : BuiltinType(TypeKind::UInt, sc) {}
};
class UInt8Type : public BuiltinType {
  friend class ASTContext;

public:
  UInt8Type(const ASTContext &sc) : BuiltinType(TypeKind::UInt8, sc) {}
};
class UInt16Type : public BuiltinType {
  friend class ASTContext;

public:
  UInt16Type(const ASTContext &sc) : BuiltinType(TypeKind::UInt16, sc) {}
};

class UInt32Type : public BuiltinType {
  friend class ASTContext;

public:
  UInt32Type(const ASTContext &sc) : BuiltinType(TypeKind::UInt32, sc) {}
};

class UInt64Type final : public BuiltinType {
  friend class ASTContext;

public:
  UInt64Type(const ASTContext &sc) : BuiltinType(TypeKind::UInt64, sc) {}
};
class UInt128Type final : public BuiltinType {
  friend class ASTContext;

public:
  UInt128Type(const ASTContext &sc) : BuiltinType(TypeKind::UInt128, sc) {}
};

class CharType final : public BuiltinType {
public:
  CharType(const ASTContext &sc) : BuiltinType(TypeKind::Char, sc) {}
};
class Char8Type final : public BuiltinType {
public:
  Char8Type(const ASTContext &sc) : BuiltinType(TypeKind::Char8, sc) {}
};
class Char16Type final : public BuiltinType {
public:
  Char16Type(const ASTContext &sc) : BuiltinType(TypeKind::Char16, sc) {}
};
class Char32Type final : public BuiltinType {
public:
  Char32Type(const ASTContext &sc) : BuiltinType(TypeKind::Char32, sc) {}
};

class BoolType final : public BuiltinType {
public:
  BoolType(const ASTContext &sc) : BuiltinType(TypeKind::Bool, sc) {}
};

class Complex32Type final : public BuiltinType {
  friend class ASTContext;

public:
  Complex32Type(const ASTContext &sc) : BuiltinType(TypeKind::Complex32, sc) {}
};

class Complex64Type final : public BuiltinType {
  friend class ASTContext;

public:
  Complex64Type(const ASTContext &sc) : BuiltinType(TypeKind::Complex64, sc) {}
};

class Imaginary32Type final : public BuiltinType {
  friend class ASTContext;

public:
  Imaginary32Type(const ASTContext &sc)
      : BuiltinType(TypeKind::Imaginary32, sc) {}
};

class Imaginary64Type final : public BuiltinType {
  friend class ASTContext;

public:
  Imaginary64Type(const ASTContext &sc)
      : BuiltinType(TypeKind::Imaginary64, sc) {}
};

class FloatType : public BuiltinType {
  friend ASTContext;

public:
  FloatType(const ASTContext &sc) : BuiltinType(TypeKind::Float, sc) {}

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;

public:
  static FloatType *Create(const ASTContext &astContext);

  static bool classof(const Type *T) { return T->GetKind() == TypeKind::Float; }
};

class Float16Type : public BuiltinType {
  friend ASTContext;

public:
  Float16Type(const ASTContext &sc) : BuiltinType(TypeKind::Float16, sc) {}
};

class Float32Type : public BuiltinType {
  friend ASTContext;

public:
  Float32Type(const ASTContext &sc) : BuiltinType(TypeKind::Float32, sc) {}
};

class Float64Type : public BuiltinType {
  friend ASTContext;

public:
  Float64Type(const ASTContext &sc) : BuiltinType(TypeKind::Float64, sc) {}
};
class Float128Type : public BuiltinType {
  friend ASTContext;

public:
  Float128Type(const ASTContext &sc) : BuiltinType(TypeKind::Float128, sc) {}
};

// // class TemplateParmType : public Type{
// // };

class FunctionType : public Type {
  QualType returnType;

public:
  FunctionType(TypeKind kind, QualType returnType, const ASTContext *canTypeCtx)
      : Type(kind, canTypeCtx) {}
};

// You are returning Type for now, it may have to be QualType
class FunType : public FunctionType,
                private llvm::TrailingObjects<FunType, QualType> {
  friend TrailingObjects;

public:
  FunType(QualType returnType, const ASTContext *sc);
};

// class NominalType : public Type {
// protected:
//   friend ASTContext;

// public:
//   // Implement isa/cast/dyncast/etc.
//   static bool classof(const Type *ty) {
//     return ty->GetKind() >= TypeKind::First_NominalType &&
//            ty->GetKind() <= TypeKind::Last_NominalType;
//   }
// };

// class StructType final : public NominalType {
// public:
// };

// class InterfaceType final : public NominalType {
// public:
// };

// class EnumType final : public NominalType {};

// class DeducedType : public Type {
// protected:
//   friend class ASTContext; // ASTContext creates these
// };

// class AutoType final : public DeducedType, public llvm::FoldingSetNode {
// public:
// };

// class ChunkType : public Type, public llvm::FoldingSetNode {};

// class AbstractPointerType : public Type, public llvm::FoldingSetNode {

//   Type pointeeType;

// public:
//   AbstractPointerType(TypeKind kind, const ASTContext &astContext)
//       : Type(kind, &astContext) {}
// };

// class PointerType : public AbstractPointerType {

//   // PointerType(Type pointeeType, Type canType)
//   //     : AbstractPointerType(Pointer, CanonicalPtr,
//   Pointee->getDependence()),
//   //       PointeeType(Pointee) {}

// public:
// };

// class MemberPointerType : public AbstractPointerType {

// public:
// };

// class AbstractReferenceType : public Type, public llvm::FoldingSetNode
// { public:
// };

// class LValueReferenceType final : public AbstractReferenceType {
// public:
// };

// class RValueReferenceType final : public AbstractReferenceType {
// public:
// };

// class ModuleType : public Type {
//   ModuleDecl *const mod;

// public:
//   /// get - Return the ModuleType for the specified module.
//   static ModuleType *Get(ModuleDecl *mod);
//   ModuleDecl *GetModule() const { return mod; }

//   // Implement isa/cast/dyncast/etc.
//   static bool classof(const Type *ty) {
//     return ty->GetKind() == TypeKind::Module;
//   }

// private:
//   ModuleType(ModuleDecl *mod, const ASTContext &sc)
//       : Type(TypeKind::Module, &sc), mod(mod) {}
// };

// class SugarType : public Type {
//   // The state of this union is known via Bits.SugarType.HasCachedType so
//   that
//   // we can avoid masking the pointer on the fast path.
//   union {
//     Type *underlyingType;
//     const ASTContext *Context;
//   };
// };
// /// An alias to a type
// /// alias Int = int; My using use using Int = int;
// class AliasType : public SugarType {
// public:
// };

// /// An alias to a type
// /// using Int = int; My using use using Int = int;
// class UsingType : public SugarType {
// public:
// };

// /// A type with a special syntax that is always sugar for a library type.
// The
// /// library type may have multiple base types. For unary syntax sugar, see
// /// UnarySyntaxSugarType.
// ///
// /// The prime examples are:
// /// Arrays: [T] -> Array<T>
// /// Dictionaries: [K : V]  -> Dictionary<K, V>
// class SyntaxSugarType : public SugarType {
// public:
// };

// /// The dictionary type [K : V], which is syntactic sugar for Dictionary<K,
// V>.
// ///
// /// Example:
// /// \code
// /// auto dict: [string : int] = ["hello" : 0, "world" : 1]
// /// \endcode
// class DictionaryType : public SyntaxSugarType {
// public:
// };

// class ArrayType : public Type, public llvm::FoldingSetNode {
// public:
// };

} // namespace stone
#endif