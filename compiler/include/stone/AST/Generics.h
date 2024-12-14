#ifndef STONE_AST_GENERICS_H
#define STONE_AST_GENERICS_H

#include "stone/AST/Decl.h"
#include "stone/AST/DeclContext.h"
#include "stone/AST/DeclName.h"
#include "stone/AST/Identifier.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {

class ASTWalker;
class GenericTypeParamDecl;

/// The kind of template argument we're storing.
// enum class TemplateArgumentKind : uint8_t {
//   /// Represents an empty template argument, e.g., one that has not
//   /// been deduced.
//   None = 0,

//   /// The template argument is a type.
//   Type,

//   /// The template argument is a declaration that was provided for a pointer,
//   /// reference, or pointer to member non-type template parameter.
//   Declaration,

//   /// The template argument is an integral value stored in an llvm::APSInt
//   /// that was provided for an integral non-type template parameter.
//   Integral,

//   /// The template argument is a template name that was provided for a
//   /// template template parameter.
//   Any,

//   /// The template argument is a pack expansion of a template name that was
//   /// provided for a template template parameter.
//   Expansion,

//   /// The template argument is an expression, and we've not resolved it to
//   one
//   /// of the other forms yet, either because it's dependent or because we're
//   /// representing a non-canonical template argument (for instance, in a
//   /// TemplateSpecializationType). Also used to represent a non-dependent
//   Expression,

//   /// The template argument is actually a parameter pack. Arguments are
//   stored
//   /// in the Args struct.
//   Pack
// };

enum class GenericRequirementKind : unsigned {
  /// A type bound T : P, where T is a type that depends on a generic
  /// parameter and P is some type that should bound T, either as a concrete
  /// supertype or a protocol to which T must conform.
  TypeConstraint,

  /// A same-type requirement T == U, where T and U are types that shall be
  /// equivalent.
  SameType,

  /// A layout bound T : L, where T is a type that depends on a generic
  /// parameter and L is some layout specification that should bound T.
  LayoutConstraint,

  // Note: there is code that packs this enum in a 2-bit bitfield.  Audit users
  // when adding enumerators.
};

class GenericRequirement final {

  SrcLoc separatorLoc;
  GenericRequirementKind kind : 2;
  bool invalid : 1;
  Type *rirstType;

  GenericRequirement(const GenericRequirement &) = delete;
  GenericRequirement &operator=(const GenericRequirement &) = delete;

public:
};

class GenericParamList final
    : private llvm::TrailingObjects<GenericParamList, GenericTypeParamDecl *> {
  friend TrailingObjects;

  SrcRange brackets;
  unsigned paramCount;
  SrcLoc whereLoc;
  llvm::MutableArrayRef<GenericRequirement> requirements;
  GenericParamList *outerParameters;

  GenericParamList(SrcLoc lAngleLoc,
                   llvm::ArrayRef<GenericTypeParamDecl *> params,
                   SrcLoc whereLoc,
                   llvm::MutableArrayRef<GenericRequirement> requirements,
                   SrcLoc rAngleLoc);

  // Don't copy.
  GenericParamList(const GenericParamList &) = delete;
  GenericParamList &operator=(const GenericParamList &) = delete;

public:
  unsigned GetParamCount() const;

public:
  bool Walk(ASTWalker &walker);
};

/// A trailing where clause.
class alignas(GenericRequirement) TrailingWhereClause final
    : private llvm::TrailingObjects<TrailingWhereClause, GenericRequirement> {
  friend TrailingObjects;

  SrcLoc whereLoc;
  SrcLoc endLoc;

  /// The number of requirements. The actual requirements are tail-allocated.
  unsigned equirementCount;

  TrailingWhereClause(SrcLoc whereLoc, SrcLoc endLoc,
                      llvm::ArrayRef<GenericRequirement> requirements);

public:
  /// Create a new trailing where clause with the given set of requirements.
  static TrailingWhereClause *
  Create(ASTContext &ctx, SrcLoc whereLoc, SrcLoc endLoc,
         llvm::ArrayRef<GenericRequirement> requirements);

  /// Retrieve the location of the 'where' keyword.
  SrcLoc GetWhereLoc() const { return whereLoc; }

  /// Retrieve the set of requirements.
  llvm::MutableArrayRef<GenericRequirement> GetRequirements() {
    return {getTrailingObjects<GenericRequirement>(), equirementCount};
  }

  /// Retrieve the set of requirements.
  llvm::ArrayRef<GenericRequirement> GetRequirements() const {
    return {getTrailingObjects<GenericRequirement>(), equirementCount};
  }

  /// Compute the source range containing this trailing where clause.
  SrcRange GetSrcRange() const { return SrcRange(whereLoc, endLoc); }

public:
  unsigned GetRequirementCount() const;

  // void Print(ColorOutputStream &OS, bool printWhereKeyword) const;
};

} // namespace stone

#endif
