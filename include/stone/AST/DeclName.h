#ifndef STONE_AST_DECLNAME_H
#define STONE_AST_DECLNAME_H

#include "stone/AST/Identifier.h"
#include "stone/AST/Type.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/OperatorKind.h"

#include "llvm/ADT/FoldingSet.h"

namespace stone {
class PrintingPolicy;

class ASTContext;
class DeclName;
class DeclNameTable;

class GenericDecl;
// class TypeSourceInfo

enum class DeclNameKind : uint8 {
  Basic = 0,
  Constructor,
  Destructor,
  Operator,
  LiteralOperator,
  UsingDirective,
};

class DeclNameBase final {
  Identifier identifier;
  /// In a special DeclName representing a subscript, this opaque pointer
  /// is used as the data of the base name identifier.
  /// This is an implementation detail that should never leak outside of
  /// DeclName.
  static const Identifier::Aligner BasicIdentifierAligner;
  /// As above, for special constructor DeclNames.
  static const Identifier::Aligner ConstructorIdentifierAligner;
  /// As above, for special destructor DeclNames.
  static const Identifier::Aligner DestructorIdentifierAligner;

  static const Identifier::Aligner OperatorIdentifierAligner;

public:
  DeclNameBase() : DeclNameBase(Identifier()) {}
  DeclNameBase(Identifier I) : identifier(I) {}

public:
  static DeclNameBase CreateConstructor() {
    return DeclNameBase(
        Identifier((const char *)&DeclNameBase::ConstructorIdentifierAligner));
  }

  static DeclNameBase CreateDestructor() {
    return DeclNameBase(
        Identifier((const char *)&DeclNameBase::DestructorIdentifierAligner));
  }

  static DeclNameBase CreateOperator() {
    return DeclNameBase(
        Identifier((const char *)&DeclNameBase::OperatorIdentifierAligner));
  }

  DeclNameKind GetKind() const {
    if (identifier.GetPointer() ==
        (const char *)&DeclNameBase::ConstructorIdentifierAligner) {
      return DeclNameKind::Constructor;
    } else if (identifier.GetPointer() ==
               (const char *)&DeclNameBase::DestructorIdentifierAligner) {
      return DeclNameKind::Destructor;
    } else if (identifier.GetPointer() ==
               (const char *)&DeclNameBase::OperatorIdentifierAligner) {
      return DeclNameKind::Operator;
    } else {
      return DeclNameKind::Basic;
    }
  }

  bool IsBasic() const { return GetKind() == DeclNameKind::Basic; }
  bool IsSpecial() const { return GetKind() != DeclNameKind::Basic; }

  // bool IsSubscript() const { return getKind() == Kind::Subscript; }

  /// Return the identifier backing the name. Assumes that the name is not
  /// special.
  Identifier GetIdentifier() const {
    assert(!IsSpecial() && "Cannot retrieve identifier from special names");
    return identifier;
  }

  bool operator==(StringRef Str) const {
    return !IsSpecial() && GetIdentifier().IsEqual(Str);
  }
  bool operator!=(StringRef Str) const { return !(*this == Str); }
  bool operator==(DeclNameBase RHS) const {
    return identifier == RHS.identifier;
  }
  bool operator!=(DeclNameBase RHS) const { return !(*this == RHS); }

  bool operator<(DeclNameBase RHS) const {
    return identifier.GetPointer() < RHS.identifier.GetPointer();
  }
  const void *GetAsOpaquePointer() const { return identifier.GetPointer(); }

  static DeclNameBase GetFromOpaquePointer(void *P) {
    return Identifier::GetFromOpaquePointer(P);
  }
};

/// Represents a compound declaration name.
struct alignas(Identifier) CompoundDeclName final
    : llvm::FoldingSetNode,
      private llvm::TrailingObjects<CompoundDeclName, Identifier> {

  friend TrailingObjects;
  friend class DeclName;

  size_t NumArgs;
  DeclNameBase nameBase;

  explicit CompoundDeclName(DeclNameBase nameBase, size_t NumArgs)
      : nameBase(nameBase), NumArgs(NumArgs) {}

  llvm::ArrayRef<Identifier> getArgumentNames() const {
    return {getTrailingObjects<Identifier>(), NumArgs};
  }
  llvm::MutableArrayRef<Identifier> getArgumentNames() {
    return {getTrailingObjects<Identifier>(), NumArgs};
  }

  /// Uniquing for the ASTContext.
  static void Profile(llvm::FoldingSetNodeID &id, DeclNameBase nameBase,
                      ArrayRef<Identifier> argumentNames);

  void Profile(llvm::FoldingSetNodeID &id) {
    Profile(id, nameBase, getArgumentNames());
  }
};

class DeclName {
  friend class NamedDecl;
  friend class ASTContext;

  // DeclNameKind declNameKind;

  /// Either a single identifier piece stored inline, or a reference to a
  /// compound declaration name.
  llvm::PointerUnion<DeclNameBase, CompoundDeclName *> nameBaseOrCompound;

  explicit DeclName(void *opaquePtr)
      : nameBaseOrCompound(
            decltype(nameBaseOrCompound)::getFromOpaqueValue(opaquePtr)) {}

public:
  DeclName() : nameBaseOrCompound(DeclNameBase()) {}

  /// Build a simple value name with one component.
  /*implicit*/
  DeclName(DeclNameBase basicName) : nameBaseOrCompound(basicName) {}

  /*implicit*/
  DeclName(Identifier basicName) : DeclName(DeclNameBase(basicName)) {}

  DeclNameBase GetDeclNameBase() const {
    if (auto compound = nameBaseOrCompound.dyn_cast<CompoundDeclName *>()) {
      return compound->nameBase;
    }
    return nameBaseOrCompound.get<DeclNameBase>();
  }

  /// Assert that the base name is not special and return its identifier.
  Identifier GetDeclNameBaseIdentifier() const {
    auto declNameBase = GetDeclNameBase();
    assert(!declNameBase.IsSpecial() &&
           "Can't retrieve the identifier of a special base name");
    return declNameBase.GetIdentifier();
  }

  // public:
  void Print(ColorStream &os, const PrintingPolicy *policy = nullptr) const;
  void Dump() const;

public:
  int Compare(DeclName other);
};

// Take a look at name look
class DeclNameTable final {
  /// Used to allocate elements in the FoldingSets below.
  // llvm::FoldingSet<DeclName> constructors;
  // llvm::FoldingSet<DeclName> destructors;

public:
  //   /// Returns the decl name constructor for the given Type.
  // Declame GetConstructorDeclName(Identifier identifier);

  // /// Returns the name of a C++ destructor for the given Type.
  // Declame GetDestructorDeclName(Identifier identifier);

  // DeclName GetSpecialName(Special::NameKind Kind,
  //                                   CanType Ty);
};

} // namespace stone

namespace llvm {

raw_ostream &operator<<(raw_ostream &OS, stone::DeclNameBase D);
raw_ostream &operator<<(raw_ostream &OS, stone::DeclName I);
// raw_ostream &operator<<(raw_ostream &OS, swift::DeclNameRef I);

// DeclBaseNames hash just like pointers.
template <> struct DenseMapInfo<stone::DeclNameBase> {
  static stone::DeclNameBase getEmptyKey() {
    return stone::Identifier::getEmptyKey();
  }
  static stone::DeclNameBase getTombstoneKey() {
    return stone::Identifier::getTombstoneKey();
  }
  static unsigned getHashValue(stone::DeclNameBase Val) {
    return DenseMapInfo<const void *>::getHashValue(Val.GetAsOpaquePointer());
  }
  static bool isEqual(stone::DeclNameBase LHS, stone::DeclNameBase RHS) {
    return LHS == RHS;
  }
};

// A DeclBaseName is "pointer like".
template <typename T> struct PointerLikeTypeTraits;
template <> struct PointerLikeTypeTraits<stone::DeclNameBase> {
public:
  static inline void *getAsVoidPointer(stone::DeclNameBase D) {
    return const_cast<void *>(D.GetAsOpaquePointer());
  }
  static inline stone::DeclNameBase getFromVoidPointer(void *P) {
    return stone::DeclNameBase::GetFromOpaquePointer(P);
  }
  enum {
    NumLowBitsAvailable =
        PointerLikeTypeTraits<stone::Identifier>::NumLowBitsAvailable
  };
};
} // end namespace llvm
#endif