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
  Compound,
};

class DeclNameBase final {
  Identifier identifier;

public:
  DeclNameBase() : DeclNameBase(Identifier()) {}
  DeclNameBase(Identifier I) : identifier(I) {}

public:
  bool IsValid() const { return !identifier.IsEmpty(); }

  // bool isSubscript() const { return getKind() == Kind::Subscript; }

  /// Return the identifier backing the name. Assumes that the name is not
  /// special.
  Identifier GetIdentifier() const {
    // assert(!IsSpecial() && "Cannot retrieve identifier from special names");
    return identifier;
  }

  // bool empty() const { return !isSpecial() && getIdentifier().empty(); }

  // bool isOperator() const {
  //   return !isSpecial() && getIdentifier().isOperator();
  // }

  // bool isEditorPlaceholder() const {
  //   return !isSpecial() && getIdentifier().isEditorPlaceholder();
  // }

  // bool hasDollarPrefix() const {
  //   return getIdentifier().hasDollarPrefix();
  // }

  /// A representation of the name to be displayed to users. May be ambiguous
  /// between identifiers and special names.
  // StringRef userFacingName() const {
  //   if (empty())
  //     return "_";

  //   switch (getKind()) {
  //   case Kind::Normal:
  //     return getIdentifier().str();
  //   case Kind::Subscript:
  //     return "subscript";
  //   case Kind::Constructor:
  //     return "init";
  //   case Kind::Destructor:
  //     return "deinit";
  //   }
  //   llvm_unreachable("unhandled kind");
  // }

  // int compare(DeclNameBase other) const {
  //   return userFacingName().compare(other.userFacingName());
  // }

  // bool operator==(StringRef Str) const {
  //   return !IsSpecial() && GetIdentifier().IsEqual(Str);
  // }

  // bool operator!=(StringRef Str) const { return !(*this == Str); }

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

// namespace detail {
// // Using, Constructor, and Destructor
// class alignas(IdentifierAlignment) SpecialDeclNameExtra
//     : public llvm::FoldingSetNode {
//   friend class DeclName;
//   friend class DeclNameTable;

//   /// The type associated with this declaration name.
//   Type ty;

// public:
//   SpecialDeclNameExtra(Type inputTy) : ty(inputTy) {}
//   void Profile(llvm::FoldingSetNodeID &ID) {
//     // TODO: ID.AddPointer(ty.GetAsOpaquePtr());
//   }
// };

// /// Contains extra information for the name of an overloaded operator
// /// in C++, such as "operator+. This do not includes literal or conversion
// /// operators. For literal operators see LiteralOperatorIdName and for
// /// conversion operators see CXXSpecialNameExtra.
// class alignas(IdentifierAlignment) OperatorIdName
//     : public detail::SpecialDeclName {
//   friend class DeclName;
//   friend class DeclNameTable;

//   /// The kind of this operator.
//   opr::OverloadedOperatorKind Kind = opr::None;
// };

// /// Contains the actual identifier that makes up the
// /// name of a C++ literal operator.
// class alignas(IdentifierAlignment) LiteralOperatorIdName
//     : public detail::SpecialDeclName,
//       public llvm::FoldingSetNode {
//   friend class DeclName;
//   friend class DeclNameTable;

//   Identifier *ID;

//   LiteralOperatorIdName(Identifier *II)
//       : SpecialDeclName(LiteralOperatorName), ID(II) {}

// public:
//   void Profile(llvm::FoldingSetNodeID &FSID) { FSID.AddPointer(ID); }
// };

// } // namespace detail

// private:
// /// In a special DeclName representing a subscript, this opaque pointer
// /// is used as the data of the base name identifier.
// /// This is an implementation detail that should never leak outside of
// /// DeclName.
// static const Identifier::Aligner SubscriptIdentifierData;
// /// As above, for special constructor DeclNames.
// static const Identifier::Aligner ConstructorIdentifierData;
// /// As above, for special destructor DeclNames.
// static const Identifier::Aligner DestructorIdentifierData;

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
  friend class ASTContext;
  DeclNameKind kind;

  /// Either a single identifier piece stored inline, or a reference to a
  /// compound declaration name.
  llvm::PointerUnion<DeclNameBase, CompoundDeclName *> nameBaseOrCompound;

  explicit DeclName(void *opaquePtr)
      : nameBaseOrCompound(
            decltype(nameBaseOrCompound)::getFromOpaqueValue(opaquePtr)) {}

public:
  DeclName() : nameBaseOrCompound(DeclNameBase()) {}

  DeclName(Identifier basicName)
      : DeclName(DeclNameKind::Basic, DeclNameBase(basicName)) {}

  DeclName(CompoundDeclName *compound)
      : kind(DeclNameKind::Compound), nameBaseOrCompound(compound) {}

  DeclName(DeclNameKind kind, DeclNameBase basicName)
      : kind(kind), nameBaseOrCompound(basicName) {}

  DeclNameBase GetDeclNameBase() const {
    if (auto compound = nameBaseOrCompound.dyn_cast<CompoundDeclName *>()) {
      return compound->nameBase;
    }
    return nameBaseOrCompound.get<DeclNameBase>();
  }

  /// Assert that the base name is not special and return its identifier.
  Identifier GetDeclNameBaseIdentifier() const {
    auto declNameBase = GetDeclNameBase();
    assert(!IsSpecial() &&
           "Can't retrieve the identifier of a special base name");
    return declNameBase.GetIdentifier();
  }

  DeclNameKind GetKind() const { return kind; }
  bool IsBasic() const { return GetKind() == DeclNameKind::Basic; }
  bool IsSpecial() const { return GetKind() != DeclNameKind::Basic; }

  // public:
  void Print(ColorStream &os, const PrintingPolicy *policy = nullptr) const;
  void Dump() const;

public:
  int Compare(DeclName other);
};

class DeclNameLoc final {
  DeclName name;

public:
  DeclNameLoc(DeclName name) : name(name) {}
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
