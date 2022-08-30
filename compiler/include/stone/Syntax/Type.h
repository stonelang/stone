#ifndef STONE_SYNTAX_RealType_H
#define STONE_SYNTAX_RealType_H

#include "stone/Foreign/Foreign.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/SyntaxAllocation.h"
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

enum class GCKind : uint8_t { None = 0, Weak, Strong };

enum class TypeQualifierKind : uint8_t {
  None = 0,
  Const = 1,
  Restrict = 2,
  Volatile = 4,
  Unaligned = 8,
  Fixed = 16
};

/// ref-qualifier associated with a function SyntaxType.
/// This determines whether a member function's "this" object can be an
/// lvalue, rvalue, or neither.
enum class RefQualifierKind : uint8_t {
  /// No ref-qualifier was provided.
  None = 0,
  /// An lvalue ref-qualifier was provided (\c &).
  LValue,

  /// An rvalue ref-qualifier was provided (\c &&).
  RValue
};
enum class ScalarTypeKind {
  Pointer,
  BlockPointer,
  MemberPointer,
  Bool,
  Integral,
  Floating,
  IntegralComplex,
  FloatingComplex,
  FixedPoint
};
class alignas(1 << TypeAlignInBits) Type
    : public SyntaxAllocation<std::aligned_storage<8, 8>::type> {

public:
};

/// const int a = 10; volatile int a = 10;
class QualType : public Type {
public:
  QualType() = default;
  QualType(const Type *ty, unsigned quals) {}

public:
  /// Retrieves a pointer to the underlying (unqualified) type.
  ///
  /// This function requires that the type not be NULL. If the type might be
  /// NULL, use the (slightly less efficient) \c getTypePtrOrNull().
  // const Type *GetTypePtr() const;
  // const Type *GetTypePtrOrNull() const;
};

// class FunctionTypeBase : public Type {};

// class FunctionType : public FunctionTypeBase {
//   // The type returned by the function.
//   QualType returnType;

// public:
//   QualType GetReturnType() { return returnType; }
// };

// class FunctionSignatureType : FunctionType {};

// class NominalType : public Type {};

// class StructType : public NominalType {};

// class EnumType : public NominalType {};

// class DeducedType : public Type {};

// class alignas(8) AutoType : public DeducedType, public llvm::FoldingSetNode {
//   friend class SyntaxContext; // SyntaxContext creates these
// };

// class BuiltinType : public Type {};

// /// An abstract base class for the two integer types.
// class BuiltinIntegerTypeBase : public BuiltinType {
//   // protected:
//   //   BuiltinIntegerTypeBase(TypeKind kind, const ASTContext &C)
//   //     : BuiltinType(kind, C) {}

//   // public:
//   //   static bool classof(const TypeBase *T) {
//   //     return T->getKind() >= TypeKind::First_AnyBuiltinIntegerType &&
//   //            T->getKind() <= TypeKind::Last_AnyBuiltinIntegerType;
//   //   }

//   // defined inline below
//   // BuiltinIntegerWidth GetWidth() const;
// };

// class BuiltinIntegerType : public BuiltinIntegerTypeBase {};

// class FloatBuiltinType : public BuiltinType {
//   friend class SyntaxContext;

// public:
//   /// IEEE floating point types.
//   enum IEEEKind {
//     IEEE16,
//     IEEE32,
//     IEEE64,
//     IEEE80,
//     IEEE128,
//   };

// private:
//   IEEEKind kind;

//   // BuiltinFloatType(IEEEKind Kind, const SyntaxContext &C)
//   //   : BuiltinType(TypeKind::BuiltinFloat, C), Kind(Kind) {}

// public:
//   IEEEKind GetIEEEKind() const { return kind; }

//   const llvm::fltSemantics &GetAPFloatSemantics() const;

//   unsigned GetBitWidth() const {
//     switch (kind) {
//     case IEEE16:
//       return 16;
//     case IEEE32:
//       return 32;
//     case IEEE64:
//       return 64;
//     case IEEE80:
//       return 80;
//     case IEEE128:
//       return 128;
//     }
//     llvm_unreachable("Invalid IEEE");
//   }
//   // static bool classof(const TypeBase *T) {
//   //   return T->getKind() == TypeKind::BuiltinFloat;
//   // }
// };

// class PointerType : public Type, public llvm::FoldingSetNode {
//   friend class SyntaxContext; // SyntaxContext creates these.

//   // QualType PointeeType;

//   // PointerType(QualType Pointee, QualType CanonicalPtr)
//   //     : Type(Pointer, CanonicalPtr, Pointee->getDependence()),
//   //       PointeeType(Pointee) {}

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
//   //   ReferenceTypeBits.SpelledAsLValue; } bool isInnerRef() const { return
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
//   //        : ReferenceType(RValueReference, Referencee, CanonicalRef, false)
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
// functions. class MemberPointerType : public Type, public llvm::FoldingSetNode
// {
//   //   friend class ASTContext; // ASTContext creates these.

//   //   QualType PointeeType;

//   //   /// The class of which the pointee is a member. Must ultimately be a
//   //   /// RecordType, but could be a typedef or a template parameter too.
//   //   const Type *Class;

//   //   MemberPointerType(QualType Pointee, const Type *Cls, QualType
//   //   CanonicalPtr)
//   //       : Type(MemberPointer, CanonicalPtr,
//   //              (Cls->getDependence() & ~TypeDependence::VariablyModified)
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
