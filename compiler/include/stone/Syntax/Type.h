#ifndef STONE_SYNTAX_Type_H
#define STONE_SYNTAX_Type_H

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

class ASTPrinter;
class CanType;
class EnumDecl;
class ModuleDecl;
class InterfaceType;
class StructDecl;
class TypeBase;
class Type;
class TypeWalker;

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

// CanType - This is a Type that is statically known to be
// canonical.  To get
/// one of these, use Type->GetCononicalType().  Since all
/// CanType's can be used as 'Type' (they just don't have sugar) we
/// derive from Type.
class CanType : public Type {

public:
};

/// const int a = 10; volatile int a = 10;
/// The qual type in this case is ust int with the aforementioned qualifiers
class QualType : public Type {
  friend class TypeQualifierCollector;

public:
  QualType() = default;
  QualType(unsigned quals) {}

public:
};

class TypeQualifierCollector final : public TypeQualifierContext {
public:
  TypeQualifierCollector(TypeQualifierContext tqc = TypeQualifierContext())
      : TypeQualifierContext(tqc) {}

public:
  /// Collect any qualifiers on the given type and return an
  /// unqualified type.  The qualifiers are assumed to be consistent
  /// with those already in the type.
  const Type *StripQualifiers(QualType type) {
    // TODO:
    //  AddFastQualifiers(type.GetLocalFastQualifiers());
    //  if (!type.HasLocalNonFastQualifiers()){
    //    return type.GetTypePtrUnsafe();
    //  }
    return nullptr;
  }
  /// Apply the collected qualifiers to the given type.
  QualType Apply(const SyntaxContext &sc, QualType qt) const;

  // THINK about this
  /// Apply the collected qualifiers to the given type.
  QualType Apply(const SyntaxContext &Context, const Type *ty) const;
};

} // namespace syn
} // namespace stone
#endif
