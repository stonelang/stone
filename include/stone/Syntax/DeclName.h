#ifndef STONE_SYNTAX_DECLNAME_H
#define STONE_SYNTAX_DECLNAME_H

#include "stone/Basic/Dumpable.h"
#include "stone/Basic/Printable.h"
#include "stone/Syntax/Identifier.h"

namespace stone {
namespace syn {

class DeclNameLoc {
public:
};

enum class DeclNameType {
  None,
  Identifier,
  Constructor,
  Destructor,
  Operator,
  LiteralOperator,
  UseDirective,
};
class DeclName : public Dumpable, public Printable {

  friend class NamedDecl;
  DeclNameType ty;

public:
  /// Construct a declaration name from an IdentifierInfo *.
  DeclName(const Identifier *identifier) {
    // TODO: SetPtrAndType(identifier, storedIdentifier);
  }

public:
  // TODO:
  bool IsIdentifier() const { return false; }
  // TODO:
  Identifier *GetAsIdentifier() const {}
  DeclNameType GetDeclNameType() { return ty; }

public:
  void Print(llvm::raw_ostream &os,
             const PrintingPolicy &policy) const override;
  void Dump() const override;

public:
  static int Compare(DeclName LHS, DeclName RHS);
};

// Constructor and Destructor
class SpecialDeclName : public DeclName {};

raw_ostream &operator<<(raw_ostream &os, DeclName name);

/// Ordering on two declaration names. If both names are identifiers,
/// this provides a lexicographical ordering.
inline bool operator<(DeclName LHS, DeclName RHS) {
  return DeclName::Compare(LHS, RHS) < 0;
}

/// Ordering on two declaration names. If both names are identifiers,
/// this provides a lexicographical ordering.
inline bool operator>(DeclName LHS, DeclName RHS) {
  return DeclName::Compare(LHS, RHS) > 0;
}

/// Ordering on two declaration names. If both names are identifiers,
/// this provides a lexicographical ordering.
inline bool operator<=(DeclName LHS, DeclName RHS) {
  return DeclName::Compare(LHS, RHS) <= 0;
}

/// Ordering on two declaration names. If both names are identifiers,
/// this provides a lexicographical ordering.
inline bool operator>=(DeclName LHS, DeclName RHS) {
  return DeclName::Compare(LHS, RHS) >= 0;
}

} // namespace syn
} // namespace stone
#endif
