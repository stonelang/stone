#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

#include "stone/Basic/Result.h"
#include "stone/Basic/SrcLoc.h"
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

class ASTPrinter;
class CanQualType;
class EnumDecl;
class ModuleDecl;
class InterfaceType;
class StructDecl;
class TypeBase;
class Type;
class TypeWalker;

// class BitterType {
// public:
// };

// class SugarType {
// public:
//   void operator==(Type T) const = delete;
//   void operator!=(Type T) const = delete;

//   void operator==(SugarType T) const = delete;
//   void operator!=(SugarType T) const = delete;
// };

// class Type final : public llvm::PointerUnion<BitterType *, SugarType *> {
// public:
//   Type() {}
// public:
//   CanType* GetCanType();
//   SugarType*  GetSugarType(); GetSugarType()->GetCanType();

// public:
//   Type Transform(llvm::function_ref<Type(Type)> fn) const;
// };

enum class GCKind : UInt8 { None = 0, Weak, Strong };

struct TypeQualifierFlags {
  enum ID : UInt8 {
    None = 0x0,
    Const = 0x1,
    Restrict = 0x2,
    Volatile = 0x4,
    Unaligned = 0x8,
    Pure = 0x18,
    Final = 0x36,
    Mutable = 0x72,


    CVRMask = Const | Volatile | Restrict,
    CVRUMask = Const | Volatile | Restrict | Unaligned
  };
};

class TypeQualifierContext {

  // bits:     |0 1 2|3|4 .. 5|6  ..  8|9   ...   31|
  //           |C R V|U|GCAttr|Lifetime|AddressSpace|
  UInt32 mask = 0;
  // static const uint32_t uMask = 0x8;
  // static const uint32_t uShift = 3;
  // static const uint32_t GCAttrMask = 0x30;
  // static const uint32_t GCAttrShift = 4;
  // static const uint32_t lifetimeMask = 0x1C0;
  // static const uint32_t lifetimeShift = 6;
  // static const uint32_t addressSpaceMask =
  //     ~(CVRMask | UMask | GCAttrMask | LifetimeMask);
  // static const uint32_t AddressSpaceShift = 9;

  SrcLoc constLoc;
  SrcLoc restrictLoc;
  SrcLoc volatileLoc;
  SrcLoc pureLoc;
  SrcLoc finalLoc;
  SrcLoc mutableLoc;

public:
  enum {
    /// The maximum supported address space number.
    /// 23 bits should be enough for anyone.
    MaxAddressSpace = 0x7fffffu,

    /// The width of the "fast" qualifier mask.
    FastWidth = 3,

    /// The fast qualifier mask.
    FastMask = (1 << FastWidth) - 1
  };

public:
  TypeQualifierContext() {
    AddFinal(SrcLoc());
  }
public:
  bool HasConst() const { return mask & TypeQualifierFlags::Const; }
  bool HasConstOnly() const { return mask == TypeQualifierFlags::Const; }
  void RemoveConst() { mask &= ~TypeQualifierFlags::Const; }
  void AddConst(SrcLoc loc = SrcLoc()) {
    constLoc = loc;
    mask |= TypeQualifierFlags::Const;
  }
  SrcLoc GetConstLoc() { return constLoc; }
  
  // TODO: Think about this whole const concept now giving that you have final and mutable 
  bool HasFinal() const { return mask & TypeQualifierFlags::Final; }
  bool HasFinalOnly() const { return mask == TypeQualifierFlags::Final; }
  void RemoveFinal() { mask &= ~TypeQualifierFlags::Final; }
  void AddFinal(SrcLoc loc = SrcLoc()) {
    finalLoc = loc;
    mask |= TypeQualifierFlags::Final;
  }
  SrcLoc GetFinalLoc() { return finalLoc; }


  bool HasMutable() const { return mask & TypeQualifierFlags::Mutable; }
  bool HasMutableOnly() const { return mask == TypeQualifierFlags::Mutable; }
  void RemoveMutable() { mask &= ~TypeQualifierFlags::Mutable; }
  void AddMutable(SrcLoc loc = SrcLoc()) {
    mutableLoc = loc;
    mask |= TypeQualifierFlags::Mutable;
  }
  SrcLoc GetMutableLoc() { return mutableLoc; }


  bool HasRestrict() const { return mask & TypeQualifierFlags::Restrict; }
  bool HasRestrictOnly() const { return mask == TypeQualifierFlags::Restrict; }
  void RemoveRestrict() { mask &= ~TypeQualifierFlags::Restrict; }
  void AddRestrict(SrcLoc loc = SrcLoc()) {
    restrictLoc = loc;
    mask |= TypeQualifierFlags::Restrict;
  }
  SrcLoc GetRestrictLoc() { return restrictLoc; }

  bool HasVolatile() const { return mask & TypeQualifierFlags::Volatile; }
  bool HasVolatileOnly() const { return mask == TypeQualifierFlags::Volatile; }
  void RemoveVolatile() { mask &= ~TypeQualifierFlags::Volatile; }
  void AddVolatile(SrcLoc loc = SrcLoc()) {
    volatileLoc = loc;
    mask |= TypeQualifierFlags::Volatile;
  }

  bool HasPure() const { return mask & TypeQualifierFlags::Pure; }
  bool HasPureOnly() const { return mask == TypeQualifierFlags::Pure; }
  void RemovePure() { mask &= ~TypeQualifierFlags::Pure; }
  void AddPure(SrcLoc loc = SrcLoc()) {
    pureLoc = loc;
    mask |= TypeQualifierFlags::Pure;
  }

  bool HasAnyTypeQualifier() {
    return (HasConst() || HasRestrict() || HasVolatile() || HasPure());
  }
  bool HasAllTypeQualifiers() {
    return (HasConst() && HasRestrict() && HasVolatile() && HasPure());
  }
  SrcLoc GetVolatileLoc() { return volatileLoc; }

  // bool HasCVR() const { return getCVRQualifiers(); }
  // unsigned GetCVR() const { return mask & CVRmask; }

  // unsigned GetCVRU() const { return mask & (CVRMask | UMask); }

  // void setCVRQualifiers(unsigned mask) {
  //   assert(!(mask & ~CVRMask) && "bitmask contains non-CVR bits");
  //   Mask = (Mask & ~CVRMask) | mask;
  // }
  // void removeCVRQualifiers(unsigned mask) {
  //   assert(!(mask & ~CVRMask) && "bitmask contains non-CVR bits");
  //   Mask &= ~mask;
  // }
  // void removeCVRQualifiers() {
  //   removeCVRQualifiers(CVRMask);
  // }
  // void addCVRQualifiers(unsigned mask) {
  //   assert(!(mask & ~CVRMask) && "bitmask contains non-CVR bits");
  //   Mask |= mask;
  // }
};

/// ref-qualifier associated with a function SyntaxType.
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

public:
  Type(TypeBase *typePtr = 0) : typePtr(typePtr) {}

public:
  bool IsNull() const { return typePtr == nullptr; }
  TypeBase *GetPtr() const { return typePtr; }

  TypeBase *operator->() const {
    assert(typePtr && "Cannot dereference a null Type!");
    return typePtr;
  }
  explicit operator bool() const { return typePtr != 0; }
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
/// const int a = 10; volatile int a = 10;
/// The qual type in this case is ust int with the aforementioned qualifiers
class QualType : public Type {
  friend class TypeQualifierCollector;

  // Thankfully, these are efficiently composable.
  // llvm::PointerIntPair<const Type *, TypeQualifierContext::FastWidth>
  //     typeAndFastWidth;
  unsigned quals;

public:
  QualType() = default;

  explicit QualType(TypeBase *ty, unsigned quals) : Type(ty), quals(quals) {
    assert(IsQualTypeOrNull() &&
           "Forming a QualType out of a unqualified type!");
  }
  // TODO: come back to this -- it seems tha we should make this into a type and
  // just pass the quals -- no need to pass the type as a separate parm
  explicit QualType(Type ty, unsigned quals) : Type(ty), quals(quals) {
    assert(IsQualTypeOrNull() &&
           "Forming a QualType out of a an unqualified type!");
  }

public:
  bool HasConst() const;
  bool AddConst();

  bool HasRestrict() const;
  bool HasVolatile() const;
  bool HasPure() const;

  bool HasQuals() const;
  bool IsCanonical() const;

  /// Return true fro now
  bool IsQualTypeOrNull() { return true; }

  // Type* GetCanType() const;

  /// Return true if this QualType doesn't point to a type yet.
  // bool IsNull() const { return ptrInt.getPointer().IsNull(); }

public:
};

// CanQualType - This is a Type that is statically known to be
// canonical.  To get
/// one of these, use Type->GetCanType().  Since all
/// CanType's can be used as 'Type' (they just don't have sugar) we
/// derive from Type.
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
  explicit CanType(QualType ty) : Type(ty) {
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
};

class TypeQualifierCollector final : public TypeQualifierContext {
public:
  TypeQualifierCollector(TypeQualifierContext tqc = TypeQualifierContext())
      : TypeQualifierContext(tqc) {}

public:
  /// Collect any qualifiers on the given type and return an
  /// unqualified type.  The qualifiers are assumed to be consistent
  /// with those already in the type.
  const Type *StripQualsFromType(QualType type);

  /// Apply the collected qualifiers to the given type.
  QualType ApplyQualsToType(const SyntaxContext &sc, QualType qt) const;

  // THINK about this
  /// Apply the collected qualifiers to the given type.
  QualType ApplyQualsToType(const SyntaxContext &Context, const Type *ty) const;
};

} // namespace syn
} // namespace stone
#endif
