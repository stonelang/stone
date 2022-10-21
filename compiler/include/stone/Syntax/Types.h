#ifndef STONE_SYNTAX_TYPES_H
#define STONE_SYNTAX_TYPES_H

#include "stone/Basic/STDTypeAlias.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Foreign/Foreign.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeChunk.h"
#include "stone/Syntax/TypeKind.h"
#include "stone/Syntax/TypeQualifier.h"

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
class CanType;

class alignas(1 << TypeAlignInBits) TypeBase
    : public SyntaxAllocation<std::aligned_storage<8, 8>::type> {

  friend class SyntaxContext;

  TypeBase(const TypeBase &) = delete;
  void operator=(const TypeBase &) = delete;

  TypeKind kind;
  TypeQualifierList *qualifiers = nullptr;
  TypeChunkList *chunks = nullptr;

  /// This union contains to the ASTContext for canonical types, and is
  /// otherwise lazily populated by ASTContext when the canonical form of a
  /// non-canonical type is requested. The disposition of the union is stored
  /// outside of the union for performance. See Bits.Type.IsCanonical.
  union {
    CanType canType;
    const SyntaxContext *sc;
  };

protected:
  union {
    uint64_t OpaqueBits;
    /// Kind - The discriminator that indicates what subclass of type this is.
    // STONE_INLINE_BITFIELD_BASE(Type, stone::BitMax(NumTypeKindBits, 8),

    //   Kind :stone::BitMax(NumTypeKindBits, 8),
    //   /// Whether this type is canonical or not.
    //   IsCanonical : 1
    // );

  } Bits;

private:
  CanType ComputeCanType();

public:
public:
  TypeBase(TypeKind kind, TypeQualifierList *qualifiers, TypeChunkList *chunks,
           const SyntaxContext *canTypeContext)
      : kind(kind), qualifiers(qualifiers), chunks(chunks), sc(nullptr) {

    /// TODO: I do not like this ....
    if (canTypeContext) {
      // Bits.Type.IsCanonical = true;
      sc = canTypeContext;
    }
  }

public:
  bool IsBasic();
  bool IsNominalType();

  /// getASTContext - Return the ASTContext that this type belongs to.
  SyntaxContext &GetSyntaxContext();

public:
  TypeKind GetKind() const { return kind; }

  void SetTypeQualifiers(TypeQualifierList *inputQualifiers) {
    qualifiers = inputQualifiers;
  }
  TypeQualifierList *GetTypeQualifiers() { return qualifiers; }

  void SetTypeChunks(TypeChunkList *inputChunks) { chunks = inputChunks; }
  TypeChunkList *GetTypeChunks() { return chunks; }

  // TypeKind GetKind() const { return
  // static_cast<TypeKind>(Bits.Type.Kind); }

  // We can do this because all types are generally cannonical types.
  // CanType GetCanType();
};

class AbstractFunctionType : public TypeBase {
  Type result;

public:
  AbstractFunctionType(TypeKind kind, TypeQualifierList *qualifiers,
                       Type result, TypeChunkList *chunks,
                       const SyntaxContext *canTypeCtx)
      : TypeBase(kind, qualifiers, chunks, canTypeCtx) {}
};

// You are returning Type for now, it may have to be QualType
class FunctionType : public AbstractFunctionType,
                     private llvm::TrailingObjects<FunctionType, Type> {
  friend TrailingObjects;

public:
  FunctionType(TypeQualifierList *qualifiers, Type result,
               TypeChunkList *chunks, const SyntaxContext *sc);

public:
  /// 'Constructor' Factory Function
  static FunctionType *Create(Type result);
};

class NominalType : public TypeBase {
protected:
  friend SyntaxContext;

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

class EnumType final : public NominalType {};

class DeducedType : public TypeBase {
protected:
  friend class SyntaxContext; // SyntaxContext creates these
};

class AutoType final : public DeducedType, public llvm::FoldingSetNode {};

// class TemplateParmType : public Type{
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

class BuiltinType : public TypeBase {
protected:
  BuiltinType(TypeKind kind, TypeQualifierList *qualifiers,
              TypeChunkList *chunks, const SyntaxContext &sc)
      : TypeBase(kind, qualifiers, chunks, &sc) {}
};

// class IdentifierType : public TypeBase {
// protected:
//   TypeBase(TypeKind kind, TypeQualifierList *qualifiers, TypeChunkList
//   *chunks,
//            const SyntaxContext &sc)
//       : TypeBase(kind, qualifiers, chunks, &sc) {}
// };

using NumberBitWidthKind = NumberBitWidth::Kind;

/// An abstract base class for integers and floats
class NumberType : public BuiltinType {
  NumberBitWidthKind bitWidthKind;

public:
  NumberType(TypeKind kind, NumberBitWidthKind bitWidthKind,
             TypeQualifierList *qualifiers, TypeChunkList *chunks,
             const SyntaxContext &sc)
      : BuiltinType(kind, qualifiers, chunks, sc), bitWidthKind(bitWidthKind) {}

public:
  unsigned GetNumberBitWidth() const {
    return NumberBitWidth::GetNumberBitWidth(bitWidthKind);
  }
  bool UsePlatformNumberBitWidth() {
    return GetNumberBitWidth() == NumberBitWidth::Platform;
  }
};

class IntegerType : public NumberType {
  friend class SyntaxContext;

public:
  IntegerType(NumberBitWidthKind bitWidthKind, TypeQualifierList *qualifiers,
              TypeChunkList *chunks, const SyntaxContext &sc)
      : NumberType(TypeKind::Integer, bitWidthKind, qualifiers, chunks, sc) {}

public:
  static IntegerType *Create(NumberBitWidthKind bitWidthKind,
                             TypeQualifierList *qualifiers,
                             TypeChunkList *chunks, const SyntaxContext &sc);
};

class UIntegerType : public NumberType {
  friend class SyntaxContext;

public:
  UIntegerType(NumberBitWidthKind bitWidthKind, TypeQualifierList *qualifiers,
               TypeChunkList *chunks, const SyntaxContext &sc)

      : NumberType(TypeKind::UInteger, bitWidthKind, qualifiers, chunks, sc) {}
};

class ComplexType : public NumberType {
  friend class SyntaxContext;

public:
  ComplexType(NumberBitWidthKind bitWidthKind, TypeQualifierList *qualifiers,
              TypeChunkList *chunks, const SyntaxContext &sc)
      : NumberType(TypeKind::Complex, bitWidthKind, qualifiers, chunks, sc) {}
};
class FloatType : public NumberType {
  friend class SyntaxContext;

public:
  FloatType(NumberBitWidthKind bitWidthKind, TypeQualifierList *qualifiers,
            TypeChunkList *chunks, const SyntaxContext &sc)
      : NumberType(TypeKind::Float, bitWidthKind, qualifiers, chunks, sc) {}

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;
  static bool classof(const TypeBase *T) {
    return T->GetKind() == TypeKind::Float;
  }
};

class VoidType : public BuiltinType {
public:
  VoidType(TypeQualifierList *qualifiers, TypeChunkList *chunks,
           const SyntaxContext &sc)
      : BuiltinType(TypeKind::Void, qualifiers, chunks, sc) {}
};

class NullType : public BuiltinType {
public:
  NullType(const SyntaxContext &sc)
      : BuiltinType(TypeKind::Null, nullptr, nullptr, sc) {}
};

class AbstractPointerType : public TypeBase, public llvm::FoldingSetNode {
public:
  AbstractPointerType(TypeKind kind, TypeQualifierList *qualifiers,
                      TypeChunkList *chunks, const SyntaxContext &sc)
      : TypeBase(kind, qualifiers, chunks, &sc) {}
};

// I don not think you need this
// class PointerType : public AbstractPointerType {
//   Type pointeeType;

// public:
//   PointerType(const SyntaxContext &sc)
//       : AbstractPointerType(TypeKind::Pointer, sc) {}

// public:
//   // QualType GetPointeeType() const { return pointeeType; }
// };

class SweetType : public TypeBase {
  // The state of this union is known via Bits.SweetType.HasCachedType so that
  // we can avoid masking the pointer on the fast path.
  union {
    Type *underlyingType;
    const SyntaxContext *Context;
  };
};

/// An alias to a type
/// alias Int = int; My using use using Int = int;
class AliasType : public SweetType {};

/// A type with a special syntax that is always sugar for a library type. The
/// library type may have multiple base types. For unary syntax sugar, see
/// UnarySyntaxSweetType.
///
/// The prime examples are:
/// Arrays: [T] -> Array<T>
/// Dictionaries: [K : V]  -> Dictionary<K, V>
class SyntaxSweetType : public SweetType {};

/// The dictionary type [K : V], which is syntactic sugar for Dictionary<K, V>.
///
/// Example:
/// \code
/// auto dict: [string : int] = ["hello" : 0, "world" : 1]
/// \endcode
class DictionaryType : public SyntaxSweetType {};

// class ArrayType : public Type, public llvm::FoldingSetNode {
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

/// Base for LValueReferenceType and RValueReferenceType
// class ReferenceType : public Type, public llvm::FoldingSetNode {
//    QualType PointeeType;

// protected:
//   ReferenceType(TypeClass tc, QualType Referencee, QualType
// CanonicalRef,
//                 bool SpelledAsLValue)
//       : Type(tc, CanonicalRef, Referencee->getDependence()),
//         PointeeType(Referencee) {
//     ReferenceTypeBits.SpelledAsLValue = SpelledAsLValue;
//     ReferenceTypeBits.InnerRef = Referencee->isReferenceType();
//   }

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
//};

// /// An lvalue reference type, per C++11 [dcl.ref].
// class LValueReferenceType final : public ReferenceType {

//   friend class ASTContext; // ASTContext creates these

//   LValueReferenceType(QualType Referencee, QualType CanonicalRef,
//                       bool SpelledAsLValue)
//       : ReferenceType(LValueReference, Referencee, CanonicalRef,
//                       SpelledAsLValue) {}

// public:
//   bool isSugared() const { return false; }
//   QualType desugar() const { return QualType(this, 0); }

//   static bool classof(const Type *T) {
//     return T->getTypeClass() == LValueReference;
//   }
//};

// /// An rvalue reference type, per C++11 [dcl.ref].
// class RValueReferenceType : public ReferenceType {
//   friend class ASTContext; // ASTContext creates these

//   RValueReferenceType(QualType Referencee, QualType CanonicalRef)
//        : ReferenceType(RValueReference, Referencee, CanonicalRef,false){}

// public:
//   bool isSugared() const { return false; }
//   QualType desugar() const { return QualType(this, 0); }

//   static bool classof(const Type *T) {
//     return T->getTypeClass() == RValueReference;
//   }
//};

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

} // namespace syn
} // namespace stone
#endif
