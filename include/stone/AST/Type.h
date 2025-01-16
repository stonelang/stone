#ifndef STONE_AST_TYPE_H
#define STONE_AST_TYPE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Foreign.h"
#include "stone/AST/InlineBitfield.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/TypeAlignment.h"
#include "stone/AST/TypeChunk.h"
#include "stone/AST/TypeKind.h"
#include "stone/AST/TypeState.h"
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

class alignas(1 << TypeAlignInBits) Type
    : public ASTAllocation<std::aligned_storage<8, 8>::type> {

  friend class ASTContext;

  Type(const Type &) = delete;
  void operator=(const Type &) = delete;

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

  /// TypeState?
  Type *underlyingType;
  bool HasUnderlyingType() const { return underlyingType != nullptr; }

public:
  Type(TypeKind kind, Type *typePtr = nullptr) : underlyingType(typePtr) {
    Bits.Type.Kind = static_cast<unsigned>(kind);
  }

public:
  bool IsBasic();

public:
  /// isCanonical - Return true if this is a canonical type.
  bool IsCanType() const {
    return (Bits.Type.IsCanonical && !HasUnderlyingType());
  }

  Type *GetUnderlyingType() const { return underlyingType; }

public:
  TypeKind GetKind() const { return static_cast<TypeKind>(Bits.Type.Kind); }
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

  //\return true if the type is a nominal type
  bool IsNominalType();

  //\return true if the type is FunType
  bool IsFunType() const;
  FunType *GetFunType() const;

  bool IsStructType() const;
  StructType *GetStructType() const;
};

class BuiltinType : public Type {
protected:
  BuiltinType(TypeKind kind) : Type(kind) {
    // Bits.Type.IsBuiltin = true;
  }
};

// class IdentifierType : public Type{
// public:
// };

// // class StringType : public BuiltinType {
// // public:
// //   StringType(const ASTContext &AC) : BuiltinType(TypeKind::String, AC) {}
// // };

class VoidType : public BuiltinType {
public:
  VoidType() : BuiltinType(TypeKind::Void) {}

public:
  static VoidType *Create(const ASTContext &astContext);
};

class NullType : public BuiltinType {
public:
  NullType() : BuiltinType(TypeKind::Null) {}
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

protected:
public:
  NumberType(TypeKind kind) : BuiltinType(kind) {}

public:
  BitWidth GetBitWidth() const;
  bool IsSigned() const {
    return false; // TODO:
  }
  bool IsFloat() const { return false; }
  bool IsImaginary() const { return false; }
  bool IsComplex() const { return false; }
  // Power();
};

class IntType : public NumberType {
  friend ASTContext;

public:
  IntType() : NumberType(TypeKind::Int) {}

public:
  static IntType *Create(const ASTContext &AC);
};

class Int8Type : public NumberType {
  friend ASTContext;

public:
  Int8Type() : NumberType(TypeKind::Int8) {}

public:
  static Int8Type *Create(const ASTContext &AC);
};

class Int16Type : public NumberType {
  friend ASTContext;

public:
  Int16Type() : NumberType(TypeKind::Int16) {}

public:
  static Int16Type *Create(const ASTContext &AC);
};

class Int32Type : public NumberType {
  friend ASTContext;

public:
  Int32Type() : NumberType(TypeKind::Int32) {}

public:
  static Int16Type *Create(const ASTContext &AC);
};

class Int64Type : public NumberType {
  friend ASTContext;

public:
  Int64Type() : NumberType(TypeKind::Int64) {}

public:
  static Int64Type *Create(const ASTContext &AC);
};

class Int128Type : public NumberType {
  friend ASTContext;

public:
  Int128Type() : NumberType(TypeKind::Int128) {}

public:
  static Int128Type *Create(const ASTContext &AC);
};

class UIntType : public NumberType {
  friend class ASTContext;

public:
  UIntType() : NumberType(TypeKind::UInt) {}
};
class UInt8Type : public NumberType {
  friend class ASTContext;

public:
  UInt8Type() : NumberType(TypeKind::UInt8) {}
};
class UInt16Type : public NumberType {
  friend class ASTContext;

public:
  UInt16Type() : NumberType(TypeKind::UInt16) {}
};

class UInt32Type : public NumberType {
  friend class ASTContext;

public:
  UInt32Type() : NumberType(TypeKind::UInt32) {}
};

class UInt64Type final : public NumberType {
  friend class ASTContext;

public:
  UInt64Type() : NumberType(TypeKind::UInt64) {}
};
class UInt128Type final : public NumberType {
  friend class ASTContext;

public:
  UInt128Type() : NumberType(TypeKind::UInt128) {}
};

class Complex32Type final : public NumberType {
  friend class ASTContext;

public:
  Complex32Type() : NumberType(TypeKind::Complex32) {}
};

class Complex64Type final : public NumberType {
  friend class ASTContext;

public:
  Complex64Type() : NumberType(TypeKind::Complex64) {}
};

class Imaginary32Type final : public NumberType {
  friend class ASTContext;

public:
  Imaginary32Type() : NumberType(TypeKind::Imaginary32) {}
};

class Imaginary64Type final : public NumberType {
  friend class ASTContext;

public:
  Imaginary64Type() : NumberType(TypeKind::Imaginary64) {}
};

class FloatType : public NumberType {
  friend ASTContext;

public:
  FloatType() : NumberType(TypeKind::Float) {}

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;

public:
  static FloatType *Create(const ASTContext &astContext);

  static bool classof(const Type *T) { return T->GetKind() == TypeKind::Float; }
};

class Float16Type : public NumberType {
  friend ASTContext;

public:
  Float16Type() : NumberType(TypeKind::Float16) {}
};

class Float32Type : public NumberType {
  friend ASTContext;

public:
  Float32Type() : NumberType(TypeKind::Float32) {}
};

class Float64Type : public NumberType {
  friend ASTContext;

public:
  Float64Type() : NumberType(TypeKind::Float64) {}
};
class Float128Type : public NumberType {
  friend ASTContext;

public:
  Float128Type() : NumberType(TypeKind::Float128) {}
};

class CharType final : public BuiltinType {
public:
  CharType() : BuiltinType(TypeKind::Char) {}
};

class Char8Type final : public BuiltinType {
public:
  Char8Type() : BuiltinType(TypeKind::Char8) {}
};

class Char16Type final : public BuiltinType {
public:
  Char16Type() : BuiltinType(TypeKind::Char16) {}
};

class Char32Type final : public BuiltinType {
public:
  Char32Type() : BuiltinType(TypeKind::Char32) {}
};

class StringType final : public BuiltinType {
public:
  StringType() : BuiltinType(TypeKind::String) {}

public:
  size_t GetLength() const;
};

class BoolType final : public BuiltinType {
public:
  BoolType() : BuiltinType(TypeKind::Bool) {}
};

// // class TemplateParmType : public Type{
// // };

class FunctionType : public Type {
  TypeState *returnType;

public:
  FunctionType(TypeKind kind, TypeState *returnType)
      : Type(kind), returnType(returnType) {}
};

// You are returning Type for now, it may have to be Type
class FunType : public FunctionType,
                private llvm::TrailingObjects<FunType, TypeState> {
  friend TrailingObjects;

public:
  FunType(TypeState *resultType);
};

class NominalType : public Type {
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

class DeducedType : public Type {
protected:
  friend class ASTContext; // ASTContext creates these
};
// own auto in = new int
class AutoType final : public DeducedType, public llvm::FoldingSetNode {
public:
};

class AccessType : public Type {
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

class SugarType : public Type {
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