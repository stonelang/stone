#ifndef STONE_AST_TYPE_H
#define STONE_AST_TYPE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/CanType.h"
#include "stone/AST/Foreign.h"
#include "stone/AST/InlineBitfield.h"
#include "stone/AST/Ownership.h"
#include "stone/AST/TypeCollector.h"
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
namespace ast {

class Type;
class CanType;
class QualType;
class TypeWalker;
class SweetType;
class ASTPrinter;
class EnumDecl;
class ModuleDecl;
class InterfaceType;
class StructDecl;

enum class GCKind : UInt8 { None = 0, Weak, Strong };

/// ref-qualifier associated with a function Type.
/// This determines whether a member function's "this" object can be an
/// lvalue, rvalue, or neither.
enum class RefQualifierKind : UInt8 {
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

class Type;
class QualType {
  friend class TypeCollector;

  Type *typePtr = nullptr;

  TypeQualifier constTypeQual{TypeQualifierKind::Const};
  TypeQualifier restrictTypeQual{TypeQualifierKind::Restrict};
  TypeQualifier volatileTypeQual{TypeQualifierKind::Volatile};
  TypeQualifier pureTypeQual{TypeQualifierKind::Pure};
  TypeQualifier immutableTypeQual{TypeQualifierKind::Immutable};
  TypeQualifier mutableTypeQual{TypeQualifierKind::Mutable};

public:
  QualType(Type *typePtr = nullptr) : typePtr(typePtr) {}

  // QualType(Type *typePtr, TypeChunkList *thunks = nullptr)
  //     : typePtr(typePtr), thunks(thunks) {}

public:
  bool IsNull() const { return typePtr == nullptr; }
  Type *GetPtr() const { return typePtr; }

  TypeKind GetKind() const;

  Type *operator->() const {
    assert(typePtr && "Cannot dereference a null Type!");
    return typePtr;
  }
  explicit operator bool() const { return typePtr != nullptr; }

public:
  /// Walk this Type.
  ///
  /// Returns true if the walk was aborted.
  // bool Walk(TypeWalker &walker) const;
  // bool Walk(TypeWalker &&walker) const { return Walk(walker); }

public:
  /// Look through the given Type and its children to find a Type
  /// for which the given predicate returns true.
  ///
  /// \param pred A predicate function object. It should return true if the give
  /// Type node satisfies the criteria.
  ///
  /// \returns true if the predicate returns true for the given Type or
  /// any of its children.
  bool FindIf(llvm::function_ref<bool(QualType)> pred) const;

  /// Transform the given Type by applying the user-provided function to
  /// each Type.
  ///
  /// This routine applies the given function to transform one Type into
  /// another. If the function leaves the Type unchanged, recurse into the
  /// child Type nodes and transform those. If any child Type node
  /// changes, the parent Type node will be rebuilt.
  ///
  /// If at any time the function returns a null Type, the null will be
  /// propagated out.
  ///
  /// \param fn A function object with the signature \c Type(Type),
  /// which accepts a Type and returns either a transformed Type or
  /// a null Type.
  ///
  /// \returns the result of transforming the Type.
  QualType Transform(llvm::function_ref<QualType(QualType)> fn) const;

  /// Transform the given Type by applying the user-provided function to
  /// each Type.
  ///
  /// This routine applies the given function to transform one Type into
  /// another. If the function leaves the Type unchanged, recurse into the
  /// child Type nodes and transform those. If any child Type node
  /// changes, the parent Type node will be rebuilt.
  ///
  /// If at any time the function returns a null Type, the null will be
  /// propagated out.
  ///
  /// If the function returns \c None, the transform operation will
  ///
  /// \param fn A function object with the signature
  /// \c Optional<Type>(Type *), which accepts a Type
  /// pointer and returns a transformed Type, a null Type (which
  /// will propagate the null Type to the outermost \c transform() call),
  /// or None (to indicate that the transform operation should recursively
  /// transform the subTypes). The function object should use \c dyn_cast
  /// rather \c getAs, because the transform itself handles desugaring.
  ///
  /// \returns the result of transforming the Type.
  QualType
  TransformRec(llvm::function_ref<llvm::Optional<QualType>(Type *)> fn) const;

  /// Look through the given Type and its children and apply fn to them.
  void Visit(llvm::function_ref<void(QualType)> fn) const {
    FindIf([&fn](QualType t) -> bool {
      fn(t);
      return false;
    });
  }

  /// Replace references to substitutable Types with new, concrete
  /// Types and return the substituted result.
  ///
  /// \param substitutions The mapping from substitutable Types to their
  /// replacements and conformances.
  ///
  /// \param options Options that affect the substitutions.
  ///
  /// \returns the substituted Type, or a null Type if an error
  /// occurred.
  // Type Substitute(SubstitutionMap substitutions,
  //                 SubstitutionOptions options = None) const;

  // /// Replace references to substitutable Types with new, concrete
  // Types and
  /// return the substituted result.
  ///
  /// \param substitutions A function mapping from substitutable Types to
  /// their replacements.
  ///
  /// \param conformances A function for looking up conformances.
  ///
  /// \param options Options that affect the substitutions.
  ///
  /// \returns the substituted Type, or a null Type if an error
  /// occurred.
  // Type Substitute(TypeSubstitutionFn substitutions,
  //                 LookupConformanceFn conformances,
  //                 SubstOptions options = None) const;

public:
  TypeQualifier &GetConst() { return constTypeQual; }
  TypeQualifier &GetRestrict() { return restrictTypeQual; }
  TypeQualifier &GetVolatile() { return volatileTypeQual; }
  TypeQualifier &GetPure() { return pureTypeQual; }
  TypeQualifier &GetImmutable() { return immutableTypeQual; }
  TypeQualifier &GetMutable() { return mutableTypeQual; }

public:
  bool HasAny() {
    return GetConst().IsValid() || GetRestrict().IsValid() ||
           GetVolatile().IsValid() || GetPure().IsValid() ||
           GetImmutable().IsValid() || GetMutable().IsValid();
  }
  void ClearAll() {
    GetConst().Clear();
    GetRestrict().Clear();
    GetVolatile().Clear();
    GetPure().Clear();
    GetImmutable().Clear();
    GetMutable().Clear();
  }

  // private:
  //   // Direct comparison is disabled for types, because they may not be
  //   canonical. void operator==(QualType T) const = delete; void
  //   operator!=(QualType T) const = delete;
};

class CanType final : public QualType {
public:
  /// Constructs a NULL canonical type.
  CanType() = default;

public:
  explicit CanType(Type *typePtr) : QualType(typePtr) {

    //   assert(IsCanTypeOrNull() &&
    //          "Forming a CanType out of a non-canonical type!");
  }

  // explicit CanType(Type ty) : Type(ty) {
  //   assert(IsCanTypeOrNull() &&
  //          "Forming a CanType out of a non-canonical type!");
  // }

  // explicit CanType(Type *ty, TypeQualifierList *quals,
  //                  TypeChunkList *thunks)
  //     : CanType(ty, quals, thunks) {
  //   assert(IsCanTypeOrNull() &&
  //          "Forming a CanType out of a non-canonical type!");
  // }
  // explicit CanType(QualType ty) : QualType(ty) {
  //   assert(IsCanTypeOrNull() &&
  //          "Forming a CanType out of a non-canonical type!");
  // }
private:
  // bool IsCanTypeOrNull() const { return true; }

  // public:
  //   void Visit(llvm::function_ref<void(CanType)> fn) const {
  //     FindIf([&fn](QualType t) -> bool {
  //       fn(CanType(t));
  //       return false;
  //     });
  //   }
  //   bool FindIf(llvm::function_ref<bool(CanType)> fn) const {
  //     return QualType::FindIf([&fn](QualType t) { return fn(CanType(t)); });
  //   }

public:
  // Direct comparison is allowed for CanTypes - they are known canonical.
  // bool operator==(CanType T) const { return GetPtr() == T.GetPtr(); }
  // bool operator!=(CanType T) const { return !operator==(T); }
  // bool operator<(CanType T) const { return GetPtr() < T.GetPtr(); }
};

class TypeCollector final {
  QualType qualType;

  TypeSpecifierCollector typeSpecifierCollector;
  TypeQualifierCollector typeQualifierCollector;
  TypeChunkCollector typeChunkCollector;
  TypeOperatorCollector typeOperatorCollector;

public:
  TypeCollector() {}

  TypeQualifierCollector &GetTypeQualifierCollector() {
    return typeQualifierCollector;
  }
  const TypeQualifierCollector &GetTypeQualifierCollector() const {
    return typeQualifierCollector;
  }
  TypeSpecifierCollector &GetTypeSpecifierCollector() {
    return typeSpecifierCollector;
  }
  const TypeSpecifierCollector &GetTypeSpecifierCollector() const {
    return typeSpecifierCollector;
  }

  TypeChunkCollector &GetTypeChunkCollector() { return typeChunkCollector; }
  const TypeChunkCollector &GetTypeChunkCollector() const {
    return typeChunkCollector;
  }
  TypeOperatorCollector &GetTypeOperatorCollector() {
    return typeOperatorCollector;
  }
  const TypeOperatorCollector &GetTypeOperatorCollector() const {
    return typeOperatorCollector;
  }

public:
  void SetType(QualType qualType);
  QualType GetType();

public:
  QualType Apply(const ast::ASTContext &astContext, QualType qualType) const;
};

class alignas(1 << TypeAlignInBits) Type
    : public ASTAllocation<std::aligned_storage<8, 8>::type> {

  friend class ASTContext;

  Type(const Type &) = delete;
  void operator=(const Type &) = delete;

  TypeKind kind;

  /// canType - This field is always set to the ASTContext for canonical
  /// types, and is otherwise lazily populated by ASTContext when the canonical
  /// form of a non-canonical type is requested.
  llvm::PointerUnion<Type *, const ASTContext *> canonicalType;

protected:
  union {
    uint64_t OpaqueBits;
    STONE_INLINE_BITFIELD_BASE(Type, stone::BitMax(NumTypeKindBits, 8) + 1 + 1,
                               Kind
                               : stone::BitMax(NumTypeKindBits, 8),

                                 /// Whether this type is canonical or not.
                                 IsCanonical : 1,
                                 // Whether this type can have qualifiers
                                 AllowQuals : 1);

    STONE_INLINE_BITFIELD(SweetType, Type, 1, HasCachedType : 1);

  } Bits;

public:
  Type(TypeKind kind, const ASTContext *canType)
      : kind(kind), canonicalType((Type *)nullptr) {
    Bits.Type.Kind = static_cast<unsigned>(kind);
    if (canType) {
      canonicalType = canType;
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

  bool AllowQuals() const { return Bits.Type.AllowQuals; }

  bool HasQuals() const;

  /// isCanonical - Return true if this is a canonical type.
  bool IsCanType() const { return canonicalType.is<const ASTContext *>(); }

  /// hasCanonicalTypeComputed - Return true if we've already computed a
  /// canonical version of this type.
  bool IsCanTypeComputed() const { return !canonicalType.isNull(); }

private:
  CanType ComputeCanType();

public:
  bool IsBuiltinType() const;
  bool IsFunType() const;
  bool IsStructType() const;
  bool IsPointerType() const;
  bool IsReferenceType() const;

public:
};

// TODO: Think about
//  class AnyType : public Type {
//  public:

//   AnyType(TypeKind kind, ASTContext *canType)
//       : Type(kind, canType) {}
// };

class FunctionType : public Type {
  QualType result;

public:
  FunctionType(TypeKind kind, QualType result, const ASTContext *canType)
      : Type(kind, canType) {}
};

// You are returning Type for now, it may have to be QualType
class FunType : public FunctionType,
                private llvm::TrailingObjects<FunType, QualType> {
  friend TrailingObjects;

public:
  FunType(QualType result, const ASTContext *astContext);
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

class InterfaceType final : public NominalType {
public:
};

class EnumType final : public NominalType {};

class DeducedType : public Type {
protected:
  friend class ASTContext; // ASTContext creates these
};

class AutoType final : public DeducedType, public llvm::FoldingSetNode {
public:
};

// class TemplateParmType : public Type{
// };

class BuiltinType : public Type {
protected:
  BuiltinType(TypeKind kind, const ASTContext &astContext)
      : Type(kind, &astContext) {}
};

class IdentifierType : public Type {};

class ScalarType : public BuiltinType {
public:
  ScalarType(TypeKind kind, const ASTContext &astContext)
      : BuiltinType(kind, astContext) {}
};

class CharType : public ScalarType {
public:
  CharType(const ASTContext &astContext)
      : ScalarType(TypeKind::Char, astContext) {}
};

class BoolType : public ScalarType {
public:
  BoolType(const ASTContext &astContext)
      : ScalarType(TypeKind::Bool, astContext) {}
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
             const ASTContext &astContext)
      : ScalarType(kind, astContext), bitWidthKind(bitWidthKind) {}

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
  IntegerType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext)
      : NumberType(TypeKind::Integer, bitWidthKind, astContext) {}

public:
  static IntegerType *Create(NumberBitWidthKind bitWidthKind,
                             const ASTContext &astContext);
};

class UIntegerType : public NumberType {
  friend class ASTContext;

public:
  UIntegerType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext)

      : NumberType(TypeKind::UInteger, bitWidthKind, astContext) {}
};

class ComplexType : public NumberType {
  friend class ASTContext;

public:
  ComplexType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext)
      : NumberType(TypeKind::Complex, bitWidthKind, astContext) {}
};

class ImaginaryType : public NumberType {
  friend class ASTContext;

public:
  ImaginaryType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext)
      : NumberType(TypeKind::Imaginary, bitWidthKind, astContext) {}
};

class FloatType : public NumberType {
  friend class ASTContext;

public:
  FloatType(NumberBitWidthKind bitWidthKind, const ASTContext &astContext)
      : NumberType(TypeKind::Float, bitWidthKind, astContext) {}

public:
  static FloatType *Create(NumberBitWidthKind bitWidthKind,
                           const ASTContext &astContext);

public:
  const llvm::fltSemantics &GetAPFloatSemantics() const;
  static bool classof(const Type *T) { return T->GetKind() == TypeKind::Float; }
};

class VoidType : public BuiltinType {
public:
  VoidType(const ASTContext &astContext)
      : BuiltinType(TypeKind::Void, astContext) {}

public:
  static VoidType *Create(const ASTContext &astContext,
                          AllocationArena arena = AllocationArena::Permanent);
};

class NullType : public BuiltinType {
public:
  NullType(const ASTContext &astContext)
      : BuiltinType(TypeKind::Null, astContext) {}
};

class ChunkType : public Type, public llvm::FoldingSetNode {};

class AbstractPointerType : public Type, public llvm::FoldingSetNode {
public:
  AbstractPointerType(TypeKind kind, const ASTContext &astContext)
      : Type(kind, &astContext) {}
};

class PointerType : public AbstractPointerType {
public:
};

class MemberPointerType : public AbstractPointerType {
public:
};

class ReferenceType : public Type, public llvm::FoldingSetNode {};

class LValueReferenceType final : public ReferenceType {};

class RValueReferenceType final : public ReferenceType {};

class ModuleType : public Type {
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
      : Type(TypeKind::Module, &astContext), mod(mod) {}
};

class SweetType : public Type {
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

/// A type with a special asttax that is always sugar for a library type. The
/// library type may have multiple base types. For unary asttax sugar, see
/// UnaryASTSweetType.
///
/// The prime examples are:
/// Arrays: [T] -> Array<T>
/// Dictionaries: [K : V]  -> Dictionary<K, V>
class ASTSweetType : public SweetType {
public:
};

/// The dictionary type [K : V], which is asttactic sugar for Dictionary<K, V>.
///
/// Example:
/// \code
/// auto dict: [string : int] = ["hello" : 0, "world" : 1]
/// \endcode
class DictionaryType : public ASTSweetType {
public:
};

// class ArrayType : public Type, public llvm::FoldingSetNode {
// public:
// };

} // namespace ast
} // namespace stone
#endif
