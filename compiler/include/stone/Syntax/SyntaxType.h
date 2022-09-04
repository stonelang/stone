#ifndef STONE_SYNTAX_SyntaxType_H
#define STONE_SYNTAX_SyntaxType_H

#include "stone/Foreign/Foreign.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeKind.h"
#include "stone/Syntax/Types.h"

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

class TypeBase;
class TypeWalker;

class SyntaxType {
  TypeBase *typePtr = nullptr;

public:
  SyntaxType(TypeBase *typePtr = 0) : typePtr(typePtr) {}

public:
  bool IsNull() const { return typePtr == nullptr; }
  TypeBase *GetPtr() const { return typePtr; }

  TypeBase *operator->() const {
    assert(typePtr && "Cannot dereference a null SyntaxType!");
    return typePtr;
  }
  explicit operator bool() const { return typePtr != 0; }
  /// Walk this SyntaxType.
  ///
  /// Returns true if the walk was aborted.
  bool Walk(TypeWalker &walker) const;
  bool Walk(TypeWalker &&walker) const { return Walk(walker); }

public:
  /// Look through the given SyntaxType and its children to find a SyntaxType
  /// for which the given predicate returns true.
  ///
  /// \param pred A predicate function object. It should return true if the give
  /// SyntaxType node satisfies the criteria.
  ///
  /// \returns true if the predicate returns true for the given SyntaxType or
  /// any of its children.
  bool FindIf(llvm::function_ref<bool(SyntaxType)> pred) const;

  /// Transform the given SyntaxType by applying the user-provided function to
  /// each SyntaxType.
  ///
  /// This routine applies the given function to transform one SyntaxType into
  /// another. If the function leaves the SyntaxType unchanged, recurse into the
  /// child SyntaxType nodes and transform those. If any child SyntaxType node
  /// changes, the parent SyntaxType node will be rebuilt.
  ///
  /// If at any time the function returns a null SyntaxType, the null will be
  /// propagated out.
  ///
  /// \param fn A function object with the signature \c SyntaxType(SyntaxType),
  /// which accepts a SyntaxType and returns either a transformed SyntaxType or
  /// a null SyntaxType.
  ///
  /// \returns the result of transforming the SyntaxType.
  SyntaxType Transform(llvm::function_ref<SyntaxType(SyntaxType)> fn) const;

  /// Transform the given SyntaxType by applying the user-provided function to
  /// each SyntaxType.
  ///
  /// This routine applies the given function to transform one SyntaxType into
  /// another. If the function leaves the SyntaxType unchanged, recurse into the
  /// child SyntaxType nodes and transform those. If any child SyntaxType node
  /// changes, the parent SyntaxType node will be rebuilt.
  ///
  /// If at any time the function returns a null SyntaxType, the null will be
  /// propagated out.
  ///
  /// If the function returns \c None, the transform operation will
  ///
  /// \param fn A function object with the signature
  /// \c Optional<SyntaxType>(SyntaxTypeBase *), which accepts a SyntaxType
  /// pointer and returns a transformed SyntaxType, a null SyntaxType (which
  /// will propagate the null SyntaxType to the outermost \c transform() call),
  /// or None (to indicate that the transform operation should recursively
  /// transform the subSyntaxTypes). The function object should use \c dyn_cast
  /// rather \c getAs, because the transform itself handles desugaring.
  ///
  /// \returns the result of transforming the SyntaxType.
  SyntaxType TransformRec(
      llvm::function_ref<llvm::Optional<SyntaxType>(TypeBase *)> fn) const;

  /// Look through the given SyntaxType and its children and apply fn to them.
  void Visit(llvm::function_ref<void(SyntaxType)> fn) const {
    FindIf([&fn](SyntaxType t) -> bool {
      fn(t);
      return false;
    });
  }

  /// Replace references to substitutable SyntaxTypes with new, concrete
  /// SyntaxTypes and return the substituted result.
  ///
  /// \param substitutions The mapping from substitutable SyntaxTypes to their
  /// replacements and conformances.
  ///
  /// \param options Options that affect the substitutions.
  ///
  /// \returns the substituted SyntaxType, or a null SyntaxType if an error
  /// occurred.
  // SyntaxType Substitute(SubstitutionMap substitutions,
  //                 SubstitutionOptions options = None) const;

  // /// Replace references to substitutable SyntaxTypes with new, concrete
  // SyntaxTypes and
  /// return the substituted result.
  ///
  /// \param substitutions A function mapping from substitutable SyntaxTypes to
  /// their replacements.
  ///
  /// \param conformances A function for looking up conformances.
  ///
  /// \param options Options that affect the substitutions.
  ///
  /// \returns the substituted SyntaxType, or a null SyntaxType if an error
  /// occurred.
  // SyntaxType Substitute(SyntaxTypeSubstitutionFn substitutions,
  //                 LookupConformanceFn conformances,
  //                 SubstOptions options = None) const;
};

// CanSyntaxType - This is a SyntaxType that is statically known to be
// canonical.  To get
/// one of these, use SyntaxType->GetCononicalSyntaxType().  Since all
/// CanSyntaxType's can be used as 'SyntaxType' (they just don't have sugar) we
/// derive from SyntaxType.
class CanType : public SyntaxType {

public:
};

} // namespace syn
} // namespace stone
#endif
