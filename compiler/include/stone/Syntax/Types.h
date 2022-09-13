#ifndef STONE_SYNTAX_RealType_H
#define STONE_SYNTAX_RealType_H

#include "stone/Basic/STDTypeAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Foreign/Foreign.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeKind.h"

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
namespace syn {

class Type;
class TypeWalker;
class ExtQuals; // Extended Qualifiers
class QualType; // Qualified SyntaxTypes
class StructDecl;
class SyntaxType;
class CanQualType;

class alignas(1 << TypeAlignInBits) TypeBase
    : public SyntaxAllocation<std::aligned_storage<8, 8>::type> {

  friend class SyntaxContext;
  TypeBase(const TypeBase &) = delete;
  void operator=(const TypeBase &) = delete;

  TypeKind kind;

  /// This union contains to the ASTContext for canonical types, and is
  /// otherwise lazily populated by ASTContext when the canonical form of a
  /// non-canonical type is requested. The disposition of the union is stored
  /// outside of the union for performance. See Bits.TypeBase.IsCanonical.
  union {
    // CanQualType canQualType;
    const SyntaxContext *sc;
  };

public:
  TypeBase(TypeKind kind, const SyntaxContext &canTypeCtx) : kind(kind) {}

public:
  bool IsBasic();
  bool IsNominalType();

  /// getASTContext - Return the ASTContext that this type belongs to.
  SyntaxContext &GetSyntaxContext();

public:
  TypeKind GetKind() const { return kind; }
};

class AbstractFunctionType : public TypeBase {
  QualType retType;

public:
};

class FunctionType : public AbstractFunctionType {

public:
  FunctionType(TypeKind kind, QualType retType);
};

class NominalType : public TypeBase {
public:
};

class StructType final : public NominalType {
public:
};

class EnumType final : public NominalType {};

class DeducedType : public TypeBase {};

class alignas(8) AutoType final : public DeducedType,
                                  public llvm::FoldingSetNode {
  friend class SyntaxContext; // SyntaxContext creates these
};

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

class BuiltinType : public TypeBase {
protected:
  BuiltinType(TypeKind kind, const SyntaxContext &canTypeCtx)
      : TypeBase(kind, canTypeCtx) {}
};

using NumberBitWidthKind = NumberBitWidth::Kind;

/// An abstract base class for integers and floats
class AbstractNumberType : public BuiltinType {
  NumberBitWidthKind bitWidthKind;

public:
  AbstractNumberType(TypeKind kind, NumberBitWidthKind bitWidthKind,
                     const SyntaxContext &canTypeCtx)
      : BuiltinType(kind, canTypeCtx), bitWidthKind(bitWidthKind) {}

public:
  unsigned GetNumberBitWidth() const {
    return NumberBitWidth::GetNumberBitWidth(bitWidthKind);
  }
  bool UsePlatformNumberBitWidth() {
    return GetNumberBitWidth() == NumberBitWidth::Platform;
  }
};

class IntegerType : public AbstractNumberType {
  friend class SyntaxContext;

public:
  IntegerType(NumberBitWidthKind bitWidthKind, const SyntaxContext &canTypeCtx)
      : AbstractNumberType(TypeKind::Integer, bitWidthKind, canTypeCtx) {}
};

class UIntegerType : public AbstractNumberType {
  friend class SyntaxContext;

public:
  UIntegerType(NumberBitWidthKind bitWidthKind, const SyntaxContext &canTypeCtx)
      : AbstractNumberType(TypeKind::UInteger, bitWidthKind, canTypeCtx) {}
};

class FloatType : public AbstractNumberType {
  friend class SyntaxContext;

public:
  FloatType(NumberBitWidthKind fpNumberBitWidthKind,
            const SyntaxContext &canTypeCtx)
      : AbstractNumberType(TypeKind::Float, fpNumberBitWidthKind, canTypeCtx) {}

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;
  static bool classof(const TypeBase *T) {
    return T->GetKind() == TypeKind::Float;
  }
};

class VoidType : public BuiltinType {
public:
  VoidType(const SyntaxContext &canTypeCtx)
      : BuiltinType(TypeKind::Void, canTypeCtx) {}
};

class NullType : public BuiltinType {
public:
  NullType(const SyntaxContext &canTypeCtx)
      : BuiltinType(TypeKind::Null, canTypeCtx) {}
};

class AbstractPointerType : public TypeBase, public llvm::FoldingSetNode {
public:
  AbstractPointerType(TypeKind kind, const SyntaxContext &canTypeCtx)
      : TypeBase(kind, canTypeCtx) {}
};
class PointerType : public AbstractPointerType {

public:
  PointerType(const SyntaxContext &canTypeCtx)
      : AbstractPointerType(TypeKind::Pointer, canTypeCtx) {}
};

class SugarType : public TypeBase {
  // The state of this union is known via Bits.SugarType.HasCachedType so that
  // we can avoid masking the pointer on the fast path.
  union {
    TypeBase *underlyingType;
    const SyntaxContext *Context;
  };
};

/// An alias to a type
/// alias Int = int; My using use using Int = int;
class AliasType : public SugarType {};

/// A type with a special syntax that is always sugar for a library type. The
/// library type may have multiple base types. For unary syntax sugar, see
/// UnarySyntaxSugarType.
///
/// The prime examples are:
/// Arrays: [T] -> Array<T>
/// Dictionaries: [K : V]  -> Dictionary<K, V>
class SyntaxSugarType : public SugarType {};

/// The dictionary type [K : V], which is syntactic sugar for Dictionary<K, V>.
///
/// Example:
/// \code
/// auto dict: [string : int] = ["hello" : 0, "world" : 1]
/// \endcode
class DictionaryType : public SyntaxSugarType {};

// class ArrayType : public TypeBase, public llvm::FoldingSetNode {
// public:
// };

// public:
//   // QualType getPointeeType() const { return PointeeType; }

//   // bool isSugared() const { return false; }
//   // QualType desugar() const { return QualType(this, 0); }

//   // void Profile(llvm::FoldingSetNodeID &ID) {
//   //   Profile(ID, getPointeeType());
//   // }

//   // static void Profile(llvm::FoldingSetNodeID &ID, QualType Pointee) {
//   //   ID.AddPointer(Pointee.getAsOpaquePtr());
//   // }

//   // static bool classof(const Type *T) { return T->getTypeClass() ==
//   Pointer; }
// };

// /// Base for LValueReferenceType and RValueReferenceType
// class ReferenceType : public Type, public llvm::FoldingSetNode {
//   //   QualType PointeeType;

//   // protected:
//   //   ReferenceType(TypeClass tc, QualType Referencee, QualType
//   CanonicalRef,
//   //                 bool SpelledAsLValue)
//   //       : Type(tc, CanonicalRef, Referencee->getDependence()),
//   //         PointeeType(Referencee) {
//   //     ReferenceTypeBits.SpelledAsLValue = SpelledAsLValue;
//   //     ReferenceTypeBits.InnerRef = Referencee->isReferenceType();
//   //   }

//   // public:
//   //   bool isSpelledAsLValue() const { return
//   //   ReferenceTypeBits.SpelledAsLValue; } bool isInnerRef() const {
//   return
//   //   ReferenceTypeBits.InnerRef; }

//   //   QualType getPointeeTypeAsWritten() const { return PointeeType; }

//   //   QualType getPointeeType() const {
//   //     // FIXME: this might strip inner qualifiers; okay?
//   //     const ReferenceType *T = this;
//   //     while (T->isInnerRef())
//   //       T = T->PointeeType->castAs<ReferenceType>();
//   //     return T->PointeeType;
//   //   }

//   //   void Profile(llvm::FoldingSetNodeID &ID) {
//   //     Profile(ID, PointeeType, isSpelledAsLValue());
//   //   }

//   //   static void Profile(llvm::FoldingSetNodeID &ID,
//   //                       QualType Referencee,
//   //                       bool SpelledAsLValue) {
//   //     ID.AddPointer(Referencee.getAsOpaquePtr());
//   //     ID.AddBoolean(SpelledAsLValue);
//   //   }

//   //   static bool classof(const Type *T) {
//   //     return T->getTypeClass() == LValueReference ||
//   //            T->getTypeClass() == RValueReference;
//   //   }
// };

// /// An lvalue reference type, per C++11 [dcl.ref].
// class LeftValueReferenceType : public ReferenceType {
//   //   friend class ASTContext; // ASTContext creates these

//   //   LValueReferenceType(QualType Referencee, QualType CanonicalRef,
//   //                       bool SpelledAsLValue)
//   //       : ReferenceType(LValueReference, Referencee, CanonicalRef,
//   //                       SpelledAsLValue) {}

//   // public:
//   //   bool isSugared() const { return false; }
//   //   QualType desugar() const { return QualType(this, 0); }

//   //   static bool classof(const Type *T) {
//   //     return T->getTypeClass() == LValueReference;
//   //   }
// };

// /// An rvalue reference type, per C++11 [dcl.ref].
// class RightValueReferenceType : public ReferenceType {
//   //   friend class ASTContext; // ASTContext creates these

//   //   RValueReferenceType(QualType Referencee, QualType CanonicalRef)
//   //        : ReferenceType(RValueReference, Referencee, CanonicalRef,
//   false)
//   {}

//   // public:
//   //   bool isSugared() const { return false; }
//   //   QualType desugar() const { return QualType(this, 0); }

//   //   static bool classof(const Type *T) {
//   //     return T->getTypeClass() == RValueReference;
//   //   }
// };

// /// A pointer to member type per C++ 8.3.3 - Pointers to members.
// ///
// /// This includes both pointers to data members and pointer to member
// functions. class MemberPointerType : public Type, public
// llvm::FoldingSetNode
// {
//   //   friend class ASTContext; // ASTContext creates these.

//   //   QualType PointeeType;

//   //   /// The class of which the pointee is a member. Must ultimately be a
//   //   /// RecordType, but could be a typedef or a template parameter too.
//   //   const Type *Class;

//   //   MemberPointerType(QualType Pointee, const Type *Cls, QualType
//   //   CanonicalPtr)
//   //       : Type(MemberPointer, CanonicalPtr,
//   //              (Cls->getDependence() &
//   ~TypeDependence::VariablyModified)
//   |
//   //                  Pointee->getDependence()),
//   //         PointeeType(Pointee), Class(Cls) {}

//   // public:
//   //   QualType getPointeeType() const { return PointeeType; }

//   //   /// Returns true if the member type (i.e. the pointee type) is a
//   //   /// function type rather than a data-member type.
//   //   bool isMemberFunctionPointer() const {
//   //     return PointeeType->isFunctionProtoType();
//   //   }

//   /// Returns true if the member type (i.e. the pointee type) is a
//   /// data type rather than a function type.
//   // bool isMemberDataPointer() const {
//   //   return !PointeeType->isFunctionProtoType();
//   // }

//   // const Type *getClass() const { return Class; }
//   // CXXRecordDecl *getMostRecentCXXRecordDecl() const;

//   // bool isSugared() const { return false; }
//   // QualType desugar() const { return QualType(this, 0); }

//   // void Profile(llvm::FoldingSetNodeID &ID) {
//   //   Profile(ID, getPointeeType(), getClass());
//   // }

//   // static void Profile(llvm::FoldingSetNodeID &ID, QualType Pointee,
//   //                     const Type *Class) {
//   //   ID.AddPointer(Pointee.getAsOpaquePtr());
//   //   ID.AddPointer(Class);
//   // }

//   // static bool classof(const Type *T) {
//   //   return T->getTypeClass() == MemberPointer;
//   // }
// };

// using TypeRep = OpaquePtr<QualType>;
// using UnionTypeRep = UnionOpaquePtr<QualType>;

// using TypeRep = OpaquePtr<QualType>;
// using UnionTypeRep = UnionOpaquePtr<QualType>;

} // namespace syn
} // namespace stone
#endif
