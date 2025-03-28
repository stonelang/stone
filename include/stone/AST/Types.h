#ifndef STONE_AST_ABSTRACTTYPE_H
#define STONE_AST_ABSTRACTTYPE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Foreign.h"
#include "stone/AST/InlineBitfield.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/Type.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/TypeChunk.h"
#include "stone/AST/TypeKind.h"
#include "stone/Basic/Basic.h"
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

class Type;
class TypeState;
class TypeWalker;
class CanType;
class SugarType;
class Int8Type;
class Int16Type;
class FunType;
class StructType;
class ASTContext;

class alignas(1 << TypeBaseAlignInBits) TypeBase
    : public ASTAllocation<std::aligned_storage<8, 8>::type> {
  friend class ASTContext;

  const ASTContext &astContext;

  /// The underlying type
  TypeBase *underlyingType = nullptr;

  TypeBase(const TypeBase &) = delete;
  void operator=(const TypeBase &) = delete;

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
                                 AllowQualifiers : 1,

                                 IsBuiltin : 1);

  } Bits;

public:
  TypeBase(TypeKind kind, const ASTContext &astContext,
           TypeBase *underlyingType = nullptr)
      : astContext(astContext), underlyingType(underlyingType) {

    Bits.TypeBase.Kind = static_cast<unsigned>(kind);
    Bits.TypeBase.IsCanonical = true;

    if (underlyingType) {
      Bits.TypeBase.IsCanonical = false;
    }
  }

public:
  bool IsBasic();
  bool IsNominalType();

  /// GetASTContext - Return the ASTContext that this type belongs to.
  ASTContext &GetASTContext();

public:
  bool AllowQualifiers() const { return Bits.TypeBase.AllowQualifiers; }
  bool HasQualifiers() const;

  bool IsCanType() const { return Bits.TypeBase.IsCanonical; }
  bool HasCanType() { return underlyingType != nullptr; }
  TypeBase *GetCanType() { return underlyingType; }

  // void SetState(TypeState *TS) { typeState = TS; }
  // TypeState *GetState() { return typeState; }

public:
  TypeKind GetKind() const { return static_cast<TypeKind>(Bits.TypeBase.Kind); }
  llvm::StringRef GetName() const;

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
  // CanType ComputeCanType();
};

// inline bool CanType::IsCanTypeOrNull() const {
//   return IsNull() ||
//          GetPtr() == llvm::DenseMapInfo<TypeBase *>::getEmptyKey() ||
//          GetPtr() == llvm::DenseMapInfo<TypeBase *>::getTombstoneKey()
//          || GetPtr()->IsCanType();
// }

class BuiltinType : public TypeBase {
protected:
  BuiltinType(TypeKind kind, const ASTContext &AC) : TypeBase(kind, AC) {
    // Bits.TypeBase.IsBuiltin = true;
  }
};

// class IdentifierType : public TypeBase{
// public:
// };

// // class StringType : public BuiltinType {
// // public:
// //   StringType(const ASTContext &AC) : BuiltinType(TypeKind::String, AC) {}
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

enum class BitWidth : uint8_t {
  Size,   // Platform-dependent size (e.g., int, uint)
  Size8,  // 8 bits
  Size16, // 16 bits
  Size32, // 32 bits
  Size64, // 64 bits
  Size128 // 128 bits
};

class NumberType : public BuiltinType {
  friend ASTContext;

  bool IsNumberType(TypeKind kind) const;

public:
  NumberType(TypeKind kind, const ASTContext &AC) : BuiltinType(kind, AC) {}

public:
  BitWidth GetBitWidth() const;
  bool IsSigned() const;
  bool IsFloat() const;
  bool IsImaginary() const;
  bool IsComplex() const;
  // Power();
};

class IntType : public NumberType {
  friend ASTContext;

public:
  IntType(const ASTContext &AC) : NumberType(TypeKind::Int, AC) {}

public:
  static IntType *Create(const ASTContext &AC);
};

class Int8Type : public NumberType {
  friend ASTContext;

public:
  Int8Type(const ASTContext &AC) : NumberType(TypeKind::Int8, AC) {}

public:
  static Int8Type *Create(const ASTContext &AC);
};

class Int16Type : public NumberType {
  friend ASTContext;

public:
  Int16Type(const ASTContext &AC) : NumberType(TypeKind::Int16, AC) {}

public:
  static Int16Type *Create(const ASTContext &AC);
};

class Int32Type : public NumberType {
  friend ASTContext;

public:
  Int32Type(const ASTContext &AC) : NumberType(TypeKind::Int32, AC) {}

public:
  static Int16Type *Create(const ASTContext &AC);
};

class Int64Type : public NumberType {
  friend ASTContext;

public:
  Int64Type(const ASTContext &AC) : NumberType(TypeKind::Int64, AC) {}

public:
  static Int64Type *Create(const ASTContext &AC);
};

class Int128Type : public NumberType {
  friend ASTContext;

public:
  Int128Type(const ASTContext &AC) : NumberType(TypeKind::Int128, AC) {}

public:
  static Int128Type *Create(const ASTContext &AC);
};

class UIntType : public NumberType {
  friend class ASTContext;

public:
  UIntType(const ASTContext &AC) : NumberType(TypeKind::UInt, AC) {}
};
class UInt8Type : public NumberType {
  friend class ASTContext;

public:
  UInt8Type(const ASTContext &AC) : NumberType(TypeKind::UInt8, AC) {}
};
class UInt16Type : public NumberType {
  friend class ASTContext;

public:
  UInt16Type(const ASTContext &AC) : NumberType(TypeKind::UInt16, AC) {}
};

class UInt32Type : public NumberType {
  friend class ASTContext;

public:
  UInt32Type(const ASTContext &AC) : NumberType(TypeKind::UInt32, AC) {}
};

class UInt64Type final : public NumberType {
  friend class ASTContext;

public:
  UInt64Type(const ASTContext &AC) : NumberType(TypeKind::UInt64, AC) {}
};
class UInt128Type final : public NumberType {
  friend class ASTContext;

public:
  UInt128Type(const ASTContext &AC) : NumberType(TypeKind::UInt128, AC) {}
};

class Complex32Type final : public NumberType {
  friend class ASTContext;

public:
  Complex32Type(const ASTContext &AC) : NumberType(TypeKind::Complex32, AC) {}
};

class Complex64Type final : public NumberType {
  friend class ASTContext;

public:
  Complex64Type(const ASTContext &AC) : NumberType(TypeKind::Complex64, AC) {}
};

class Imaginary32Type final : public NumberType {
  friend class ASTContext;

public:
  Imaginary32Type(const ASTContext &AC)
      : NumberType(TypeKind::Imaginary32, AC) {}
};

class Imaginary64Type final : public NumberType {
  friend class ASTContext;

public:
  Imaginary64Type(const ASTContext &AC)
      : NumberType(TypeKind::Imaginary64, AC) {}
};

class FloatType : public NumberType {
  friend ASTContext;

public:
  FloatType(const ASTContext &AC) : NumberType(TypeKind::Float, AC) {}

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;

public:
  static FloatType *Create(const ASTContext &astContext);

  static bool classof(const TypeBase *T) {
    return T->GetKind() == TypeKind::Float;
  }
};

class Float16Type : public NumberType {
  friend ASTContext;

public:
  Float16Type(const ASTContext &AC) : NumberType(TypeKind::Float16, AC) {}
};

class Float32Type : public NumberType {
  friend ASTContext;

public:
  Float32Type(const ASTContext &AC) : NumberType(TypeKind::Float32, AC) {}
};

class Float64Type : public NumberType {
  friend ASTContext;

public:
  Float64Type(const ASTContext &AC) : NumberType(TypeKind::Float64, AC) {}
};
class Float128Type : public NumberType {
  friend ASTContext;

public:
  Float128Type(const ASTContext &AC) : NumberType(TypeKind::Float128, AC) {}
};

class CharType final : public BuiltinType {
public:
  CharType(const ASTContext &AC) : BuiltinType(TypeKind::Char, AC) {}
};

class Char8Type final : public BuiltinType {
public:
  Char8Type(const ASTContext &AC) : BuiltinType(TypeKind::Char8, AC) {}
};

class Char16Type final : public BuiltinType {
public:
  Char16Type(const ASTContext &AC) : BuiltinType(TypeKind::Char16, AC) {}
};

class Char32Type final : public BuiltinType {
public:
  Char32Type(const ASTContext &AC) : BuiltinType(TypeKind::Char32, AC) {}
};

class StringType final : public BuiltinType {
public:
  StringType(const ASTContext &AC) : BuiltinType(TypeKind::String, AC) {}

public:
  size_t GetLength() const;
};

class BoolType final : public BuiltinType {
public:
  BoolType(const ASTContext &AC) : BuiltinType(TypeKind::Bool, AC) {}
};

// // class TemplateParmType : public Type{
// // };

class FunctionType : public TypeBase {
  Type returnType;

public:
  FunctionType(TypeKind kind, Type returnType, const ASTContext &astContext)
      : TypeBase(kind, astContext) {}
};

// You are returning Type for now, it may have to be Type
class FunType : public FunctionType,
                private llvm::TrailingObjects<FunType, Type> {
  friend TrailingObjects;

public:
  FunType(Type resultType, const ASTContext &AC);
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

class ClassType final : public NominalType {
public:
};

class InterfaceType final : public NominalType {
public:
};

class EnumType final : public NominalType {

public:
  // GetString();
};

class DeducedType : public TypeBase {
protected:
  friend class ASTContext; // ASTContext creates these
};
// own auto in = new int
class AutoType final : public DeducedType, public llvm::FoldingSetNode {
public:
};

class AccessType : public TypeBase {
  // Base class for access-related types.
};

class PointerType : public AccessType {
  // Base class for pointer types.
};

class RawType : public PointerType {
  // General pointer type.
};

class MemberPointerType : public PointerType {
  // Specialized pointer type for members.
};

class OwnType : public PointerType {
  // Pointer type with ownership semantics.
};

class OwnRawType : public PointerType {
  // Pointer type with ownership semantics.
};

class MoveType : public PointerType {
  // Pointer type with move semantics.
};

class ReferenceType : public AccessType {
  // Base class for reference types.
};

class RefType : public ReferenceType {
  // General reference type.
};

// class PointerType : public Type {
// public:
// };

// class PointerType : public PointerType {
// public:
// };

// class MemberPointerType : public PointerType {
// public:
// };

// class TypedPointerType : public PointerType {
//   //     Type* pointeeType;

//   // public:
//   //     TypedPointer(Type* pointeeType) : pointeeType(pointeeType) {}

//   //     Type* GetPointeeType() const { return pointeeType; }
// };

// class OwnType : public PointerType {
//   // public:
//   //     OwnPointer(Type* pointeeType) : TypedPointer(pointeeType) {}

//   //     void TakeOwnership() {
//   //         // Add ownership semantics
//   //     }
// };

// class MoveType : public PointerType {
//   // public:
//   //     MovePointer(Type* pointeeType) : TypedPointer(pointeeType) {}

//   //     void MoveTo(MovePointer& target) {
//   //         // Add move semantics
//   //     }
// };

// class AbstractReferenceType : public Type {
//   // public:
//   //     virtual ~AbstractReference() = default;
// };

// class RefType : public AbstractReferenceType {
//   //     Type* referencedType;

//   // public:
//   //     ReferenceType(Type* referencedType) : referencedType(referencedType)
//   {}

//   //     Type* GetReferencedType() const { return referencedType; }
// };

// class ChunkType : public Type, public llvm::FoldingSetNode {};

// class PointerType : public Type, public llvm::FoldingSetNode {

//   Type pointeeType;

// public:
//   PointerType(TypeKind kind, const ASTContext &astContext)
//       : Type(kind, &astContext) {}
// };

// class PointerType : public PointerType {

//   // PointerType(Type pointeeType, Type canType)
//   //     : PointerType(Pointer, CanonicalPtr,
//   Pointee->getDependence()),
//   //       PointeeType(Pointee) {}

// public:
// };

// class MemberPointerType : public PointerType {

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
//   ModuleType(ModuleDecl *mod, const ASTContext &AC)
//       : Type(TypeKind::Module, &AC), mod(mod) {}
// };

class SugarType : public TypeBase {
  // The state of this union is known via Bits.SugarType.HasCachedType so
  // that
  // we can avoid masking the pointer on the fast path.
  // union {
  //   Type *underlyingType;
  //   const ASTContext *Context;
  // };
};
/// An alias to a type
/// alias Int = int; My using use using Int = int;
class AliasType : public SugarType {
public:
};

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