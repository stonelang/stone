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
};

// class AbstractFunctionType : public TypeBase {
// public:
// };

// class FunctionType : public AbstractFunctionType {
// public:
// };

// class NominalType : public TypeBase {
// public:

// };

// class StructType : public NominalType {
// public:

// };

// class EnumType : public NominalType {};

// class DeducedType : public Type {};

// class alignas(8) AutoType : public DeducedType, public llvm::FoldingSetNode {
//   friend class SyntaxContext; // SyntaxContext creates these
// };

struct BitWidth final {
  enum Kind : UInt8 {
    BitWidth8,
    BitWidth16,
    BitWidth32,
    BitWidth64,
    BitWidth80,
    BitWidth128,
  };
  static unsigned GetBitWidth(BitWidth::Kind kind) {
    switch (kind) {
    case BitWidth::BitWidth8:
      return 8;
    case BitWidth::BitWidth16:
      return 16;
    case BitWidth::BitWidth32:
      return 32;
    case BitWidth::BitWidth64:
      return 64;
    case BitWidth::BitWidth80:
      return 80;
    case BitWidth::BitWidth128:
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

/// An abstract base class for the two integer types.
class AbstractIntegerType : public BuiltinType {
  BitWidth bitWidth;

public:
};

class IntegerType : public AbstractIntegerType {
public:
};

// class UIntegerType : public AbstractIntegerType {
// public:
// };

// class FloatType : public BuiltinType {
//   friend class SyntaxContext;

// private:
//   using FloatPointBitWidth = BitWidth::Kind;
//   FloatPointBitWidth fpBitWidth;
//   // FloatType(FloatPointBitWidth fpBitWidth, const SyntaxContext &C)

// public:
//   const llvm::fltSemantics &GetAPFloatSemantics() const;

//   FloatPointBitWidth GetFloatPointBitWidth() const { return fpBitWidth; }
//   unsigned GetBitWidth() const { return BitWidth::GetBitWidth(fpBitWidth);
//   }
//   // static bool classof(const TypeBase *T) {
//   //   return T->getKind() == TypeKind::Float;
//   // }
// };

// class VoidType : public BuiltinType {
// public:
// };

// class NullType : public BuiltinType {
// public:
// };

// class PointerType : public TypeBase, public llvm::FoldingSetNode {
//   friend class SyntaxContext; // SyntaxContext creates these.
// public:
// };

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
