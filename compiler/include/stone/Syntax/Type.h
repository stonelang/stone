#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

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
class SyntaxType;
class TypeWalker;

enum class GCKind : uint8_t { None = 0, Weak, Strong };

enum class TypeQualKind : uint8_t {
  None = 0,
  Const = 1,
  Restrict = 2,
  Volatile = 4,
  Unaligned = 8,
  Fixed = 16
};

/// ref-qualifier associated with a function type.
/// This determines whether a member function's "this" object can be an
/// lvalue, rvalue, or neither.
enum class RefQualKind : uint8_t {
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
  ntegralComplex,
  FloatingComplex,
  FixedPoint
};

class ExtQuals; // Extended Qualifiers
class QualType; // Qualified Types
class StructDecl;
class Type;

class TypeContext final {
public:
};

class Type {
  SyntaxType *syntaxType = nullptr;

public:
  Type(SyntaxType *syntaxType = 0) : syntaxType(syntaxType) {}

public:
  bool IsNull() const { return syntaxType == nullptr; }
  SyntaxType *GetPointer() const { return syntaxType; }

  SyntaxType *operator->() const {
    assert(syntaxType && "Cannot dereference a null Type!");
    return syntaxType;
  }

  explicit operator bool() const { return syntaxType != 0; }

  /// Walk this type.
  ///
  /// Returns true if the walk was aborted.
  bool Walk(TypeWalker &walker) const;
  bool Walk(TypeWalker &&walker) const { return Walk(walker); }

public:
  /// Look through the given type and its children to find a type for
  /// which the given predicate returns true.
  ///
  /// \param pred A predicate function object. It should return true if the give
  /// type node satisfies the criteria.
  ///
  /// \returns true if the predicate returns true for the given type or any of
  /// its children.
  bool FindIf(llvm::function_ref<bool(Type)> pred) const;

  /// Transform the given type by applying the user-provided function to
  /// each type.
  ///
  /// This routine applies the given function to transform one type into
  /// another. If the function leaves the type unchanged, recurse into the
  /// child type nodes and transform those. If any child type node changes,
  /// the parent type node will be rebuilt.
  ///
  /// If at any time the function returns a null type, the null will be
  /// propagated out.
  ///
  /// \param fn A function object with the signature \c Type(Type), which
  /// accepts a type and returns either a transformed type or a null type.
  ///
  /// \returns the result of transforming the type.
  Type Transform(llvm::function_ref<Type(Type)> fn) const;

  /// Transform the given type by applying the user-provided function to
  /// each type.
  ///
  /// This routine applies the given function to transform one type into
  /// another. If the function leaves the type unchanged, recurse into the
  /// child type nodes and transform those. If any child type node changes,
  /// the parent type node will be rebuilt.
  ///
  /// If at any time the function returns a null type, the null will be
  /// propagated out.
  ///
  /// If the function returns \c None, the transform operation will
  ///
  /// \param fn A function object with the signature
  /// \c Optional<Type>(TypeBase *), which accepts a type pointer and returns a
  /// transformed type, a null type (which will propagate the null type to the
  /// outermost \c transform() call), or None (to indicate that the transform
  /// operation should recursively transform the subtypes). The function object
  /// should use \c dyn_cast rather \c getAs, because the transform itself
  /// handles desugaring.
  ///
  /// \returns the result of transforming the type.
  Type
  TransformRec(llvm::function_ref<llvm::Optional<Type>(SyntaxType *)> fn) const;

  /// Look through the given type and its children and apply fn to them.
  void Visit(llvm::function_ref<void(Type)> fn) const {
    FindIf([&fn](Type t) -> bool {
      fn(t);
      return false;
    });
  }

  /// Replace references to substitutable types with new, concrete types and
  /// return the substituted result.
  ///
  /// \param substitutions The mapping from substitutable types to their
  /// replacements and conformances.
  ///
  /// \param options Options that affect the substitutions.
  ///
  /// \returns the substituted type, or a null type if an error occurred.
  // Type Substitute(SubstitutionMap substitutions,
  //                 SubstitutionOptions options = None) const;

  // /// Replace references to substitutable types with new, concrete types and
  /// return the substituted result.
  ///
  /// \param substitutions A function mapping from substitutable types to their
  /// replacements.
  ///
  /// \param conformances A function for looking up conformances.
  ///
  /// \param options Options that affect the substitutions.
  ///
  /// \returns the substituted type, or a null type if an error occurred.
  // Type Substitute(TypeSubstitutionFn substitutions,
  //                 LookupConformanceFn conformances,
  //                 SubstOptions options = None) const;
};

// CanType - This is a Type that is statically known to be canonical.  To get
/// one of these, use Type->GetCononicalType().  Since all CanType's can be used
/// as 'Type' (they just don't have sugar) we derive from Type.
class CanType : public Type {
public:
};


} // namespace syn
} // namespace stone
#endif
