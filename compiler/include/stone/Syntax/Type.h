#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

#include "stone/Basic/SrcLoc.h"
#include "stone/Syntax/ASTAllocation.h"
#include "stone/Syntax/Foreign.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeKind.h"
#include "stone/Syntax/TypeQualifier.h"
#include "stone/Syntax/TypeThunk.h"

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

class ASTPrinter;
class CanQualType;
class EnumDecl;
class ModuleDecl;
class InterfaceType;
class StructDecl;
class TypeBase;
class Type;
class TypeWalker;

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

class Type {
  TypeBase *typePtr = nullptr;
  TypeQualifierCollector qualCollector;

public:
  Type(TypeBase *typePtr = nullptr,
       TypeQualifierCollector qualCollector = TypeQualifierCollector())
      : typePtr(typePtr), qualCollector(qualCollector) {}

public:
  bool IsNull() const { return typePtr == nullptr; }
  TypeBase *GetPtr() const { return typePtr; }
  TypeBase *operator->() const {
    assert(typePtr && "Cannot dereference a null Type!");
    return typePtr;
  }
  explicit operator bool() const { return typePtr != nullptr; }

public:
  void SetTypeQualifierCollector(TypeQualifierCollector collector) {
    qualCollector = collector;
  }
  TypeQualifierCollector &GetTypeQualifierCollector() { return qualCollector; }

public:
  /// Walk this Type.
  ///
  /// Returns true if the walk was aborted.
  bool Walk(TypeWalker &walker) const;
  bool Walk(TypeWalker &&walker) const { return Walk(walker); }

public:
  /// Look through the given Type and its children to find a Type
  /// for which the given predicate returns true.
  ///
  /// \param pred A predicate function object. It should return true if the give
  /// Type node satisfies the criteria.
  ///
  /// \returns true if the predicate returns true for the given Type or
  /// any of its children.
  bool FindIf(llvm::function_ref<bool(Type)> pred) const;

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
  Type Transform(llvm::function_ref<Type(Type)> fn) const;

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
  /// \c Optional<Type>(TypeBase *), which accepts a Type
  /// pointer and returns a transformed Type, a null Type (which
  /// will propagate the null Type to the outermost \c transform() call),
  /// or None (to indicate that the transform operation should recursively
  /// transform the subTypes). The function object should use \c dyn_cast
  /// rather \c getAs, because the transform itself handles desugaring.
  ///
  /// \returns the result of transforming the Type.
  Type
  TransformRec(llvm::function_ref<llvm::Optional<Type>(TypeBase *)> fn) const;

  /// Look through the given Type and its children and apply fn to them.
  void Visit(llvm::function_ref<void(Type)> fn) const {
    FindIf([&fn](Type t) -> bool {
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

private:
  // Direct comparison is disabled for types, because they may not be canonical.
  void operator==(Type T) const = delete;
  void operator!=(Type T) const = delete;
};

class QualTypeBase : public Type {

  unsigned qualifiers = 0;
  enum Flags : unsigned {
    None = 1 << 0,
    Const = 1 << 1,
    Immutable = 1 << 2,
    Mutable = 1 << 3,
    Pure = 1 << 4,
  };

public:
  QualTypeBase(TypeBase *ty) : Type(ty), qualifiers(0) {}
  QualTypeBase(Type ty) : Type(ty), qualifiers(0) {}

public:
  bool HasConst() const { return qualifiers & Flags::Const; }
  bool IsConst() const { return qualifiers == Flags::Const; }
  void RemoveConst() { qualifiers &= ~Flags::Const; }
  void AddConst() { qualifiers |= Flags::Const; }

public:
  bool HasImmutable() const { return qualifiers & Flags::Immutable; }
  bool IsImmutable() const { return qualifiers == Flags::Immutable; }
  void RemoveImmutable() { qualifiers &= ~Flags::Immutable; }
  void AddImmutable() { qualifiers |= Flags::Immutable; }

public:
  bool HasMutable() const { return qualifiers & Flags::Mutable; }
  bool IsMutable() const { return qualifiers == Flags::Mutable; }
  void RemoveMutable() { qualifiers &= ~Flags::Mutable; }
  void AddMutable() { qualifiers |= Flags::Mutable; }

public:
  bool HasPure() const { return qualifiers & Flags::Pure; }
  bool IsPure() const { return qualifiers == Flags::Pure; }
  void RemovePure() { qualifiers &= ~Flags::Pure; }
  void AddPure() { qualifiers |= Flags::Pure; }

public:
  bool HasAny() {
    return (HasConst() || HasImmutable() || HasMutable() || HasPure());
  }

  void Reset() { qualifiers = 0; }
};

class QualType final : public QualTypeBase {
public:
  QualType() = default;

public:
  QualType(TypeBase *ty) : QualTypeBase(ty) {}
  QualType(Type ty) : QualTypeBase(ty) {}
};

class CanType final : public Type {
public:
  /// Constructs a NULL canonical type.
  CanType() = default;

public:
  explicit CanType(TypeBase *ty) : Type(ty) {
    assert(IsCanTypeOrNull() &&
           "Forming a CanType out of a non-canonical type!");
  }
  explicit CanType(Type ty) : Type(ty) {
    assert(IsCanTypeOrNull() &&
           "Forming a CanType out of a non-canonical type!");
  }

private:
  bool IsCanTypeOrNull() const { return true; }

public:
  void Visit(llvm::function_ref<void(CanType)> fn) const {
    FindIf([&fn](Type t) -> bool {
      fn(CanType(t));
      return false;
    });
  }
  bool FindIf(llvm::function_ref<bool(CanType)> fn) const {
    return Type::FindIf([&fn](Type t) { return fn(CanType(t)); });
  }

public:
  // Direct comparison is allowed for CanTypes - they are known canonical.
  bool operator==(CanType T) const { return GetPtr() == T.GetPtr(); }
  bool operator!=(CanType T) const { return !operator==(T); }
  bool operator<(CanType T) const { return GetPtr() < T.GetPtr(); }
};

} // namespace stone
#endif