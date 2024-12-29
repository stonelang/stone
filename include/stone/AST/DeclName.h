#ifndef STONE_AST_DECLNAME_H
#define STONE_AST_DECLNAME_H

#include "stone/AST/Identifier.h"
#include "stone/AST/QualType.h"
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

  // bool isSubscript() const { return getKind() == Kind::Subscript; }

  /// Return the identifier backing the name. Assumes that the name is not
  /// special.
  Identifier GetIdentifier() const {
    assert(!IsSpecial() && "Cannot retrieve identifier from special names");
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

// namespace detail {
// // Using, Constructor, and Destructor
// class alignas(IdentifierAlignment) SpecialDeclNameExtra
//     : public llvm::FoldingSetNode {
//   friend class DeclName;
//   friend class DeclNameTable;

//   /// The type associated with this declaration name.
//   QualType ty;

// public:
//   SpecialDeclNameExtra(QualType inputTy) : ty(inputTy) {}
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
  //   /// Construct a declaration name from an Identifier *.
  //   DeclName(const Identifier *identifier) {
  //     SetPtrAndKind(identifier, StoredNameKind::Identifier);
  //   }

  // public:
  //   // TODO:
  //   bool IsIdentifier() const { return declNameKind ==
  //   DeclNameKind::Identifier; } bool IsConstructor() const {
  //     return declNameKind == DeclNameKind::Constructor;
  //   }
  //   bool IsDestructor() const { return declNameKind ==
  //   DeclNameKind::Destructor; } bool IsOperator() const { return declNameKind
  //   == DeclNameKind::Operator; } bool IsLiteralOperator() const {
  //     return declNameKind == DeclNameKind::LiteralOperator;
  //   }
  //   bool IsUsingDirective() const {
  //     return declNameKind == DeclNameKind::UsingDirective;
  //   }

  //   // TODO:
  //   Identifier *GetAsIdentifier() const {}
  //   DeclNameKind GetDeclNameKind() { return declNameKind; }

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

// struct DeclNameBase final {
// private:
//   /// Name - The declaration name, also encoding name kind.
//   DeclName name;

//   /// Loc - The main source location for the declaration name.
//   SrcLoc nameLoc;

//   /// Info - Further source/type location info for special kinds of names.
//   DeclNameLoc specialNameLoc;

// public:
//   DeclNameBase()
//       : name(nullptr), nameLoc(SrcLoc()), specialNameLoc(nullptr) {}

//   DeclNameBase(DeclName name, SrcLoc nameLoc)
//       : name(name), nameLoc(nameLoc), specialNameLoc(name) {}

//   DeclNameBase(DeclName name, SrcLoc nameLoc, DeclNameLoc specialNameLoc)
//       : name(name), nameLoc(nameLoc), specialNameLoc(specialNameLoc) {}

//   /// getName - Returns the embedded declaration name.
//   DeclName GetName() const { return name; }

//   /// setName - Sets the embedded declaration name.
//   void SetName(DeclName n) { name = n; }

//   /// getLoc - Returns the main location of the declaration name.
//   SrcLoc GetNameLoc() const { return nameLoc; }

//   /// setLoc - Sets the main location of the declaration name.
//   void SetNameLoc(SrcLoc loc) { nameLoc = loc; }

//   const DeclNameLoc &GetSpecialNameLoc() const { return specialNameLoc; }
//   void SetSpecialNameLoc(const DeclNameLoc &info) { specialNameLoc = info; }

// /// getNamedTypeInfo - Returns the source type info associated to
// /// the name. Assumes it is a constructor, destructor or conversion.

// TypeSourceInfo *getNamedTypeInfo() const {
//   if (Name.getNameKind() != DeclName::CXXConstructorName &&
//       Name.getNameKind() != DeclName::CXXDestructorName &&
//       Name.getNameKind() != DeclName::CXXConversionFunctionName)
//     return nullptr;
//   return LocInfo.getNamedTypeInfo();
// }

// /// setNamedTypeInfo - Sets the source type info associated to
// /// the name. Assumes it is a constructor, destructor or conversion.
// void setNamedTypeInfo(TypeSourceInfo *TInfo) {
//   assert(Name.getNameKind() == DeclName::CXXConstructorName ||
//          Name.getNameKind() == DeclName::CXXDestructorName ||
//          Name.getNameKind() == DeclName::CXXConversionFunctionName);
//   LocInfo = DeclNameLoc::makeNamedTypeLoc(TInfo);
// }

// /// getCXXOperatorNameRange - Gets the range of the operator name
// /// (without the operator keyword). Assumes it is a (non-literal) operator.
// SourceRange getCXXOperatorNameRange() const {
//   if (Name.getNameKind() != DeclName::CXXOperatorName)
//     return SourceRange();
//   return LocInfo.getCXXOperatorNameRange();
// }

// /// setCXXOperatorNameRange - Sets the range of the operator name
// /// (without the operator keyword). Assumes it is a C++ operator.
// void setCXXOperatorNameRange(SourceRange R) {
//   assert(Name.getNameKind() == DeclName::CXXOperatorName);
//   LocInfo = DeclNameLoc::makeCXXOperatorNameLoc(R);
// }

// /// getCXXLiteralOperatorNameLoc - Returns the location of the literal
// /// operator name (not the operator keyword).
// /// Assumes it is a literal operator.
// SrcLoc GetLiteralOperatorNameLoc() const {
//   if (Name.getNameKind() != DeclName::CXXLiteralOperatorName)
//     return SrcLoc();
//   return LocInfo.getCXXLiteralOperatorNameLoc();
// }

// /// setCXXLiteralOperatorNameLoc - Sets the location of the literal
// /// operator name (not the operator keyword).
// /// Assumes it is a literal operator.
// void setCXXLiteralOperatorNameLoc(SrcLoc Loc) {
//   assert(Name.getNameKind() == DeclName::CXXLiteralOperatorName);
//   LocInfo = DeclNameLoc::makeCXXLiteralOperatorNameLoc(Loc);
// }

// /// Determine whether this name involves a template parameter.
// bool isInstantiationDependent() const;

// /// Determine whether this name contains an unexpanded
// /// parameter pack.
// bool containsUnexpandedParameterPack() const;

// /// getAsString - Retrieve the human-readable string for this name.
// std::string getAsString() const;

// /// printName - Print the human-readable name to a stream.
// void printName(raw_ostream &OS, PrintingPolicy Policy) const;

// /// getBeginLoc - Retrieve the location of the first token.
// SrcLoc getBeginLoc() const { return NameLoc; }

// /// getSourceRange - The range of the declaration name.
// SourceRange getSourceRange() const LLVM_READONLY {
//   return SourceRange(getBeginLoc(), getEndLoc());
// }

// SrcLoc getEndLoc() const LLVM_READONLY {
//   SrcLoc EndLoc = getEndLocPrivate();
//   return EndLoc.isValid() ? EndLoc : getBeginLoc();
// }

// public:
//   bool IsSimple();
//   bool IsSpecial();

// private:
//   // SrcLoc getEndLocPrivate() const;
// };

/// DeclNameTable is used to store and retrieve DeclName
/// instances for the various kinds of declaration names, e.g., normal
/// identifiers, constructor names, etc. This class contains
/// uniqued versions of each of the special names, which can be
/// retrieved using its member functions (e.g., GetConstructorName).
// class DeclNameTable final {
//   /// Used to allocate elements in the FoldingSets below.
//   const ASTContext &tc;

//   /// Manage the uniqued SpecialDeclName representing stone constructors.
//   /// GetConstructorName and GetSpecialName can be used to obtain
//   /// a DeclName from the corresponding type of the constructor.
//   llvm::FoldingSet<detail::SpecialDeclNameExtra> constructorNames;

//   /// Manage the uniqued CXXSpecialNameExtra representing C++ destructors.
//   /// getCXXDestructorName and getCXXSpecialName can be used to obtain
//   /// a DeclName from the corresponding type of the destructor.
//   llvm::FoldingSet<detail::SpecialDeclNameExtra> destructorNames;

//   //
//   llvm::FoldingSet<detail::SpecialDeclNameExtra> usingNames;

/// Manage the uniqued CXXSpecialNameExtra representing C++ conversion
/// functions. getCXXConversionFunctionName and getCXXSpecialName can be
/// used to obtain a DeclName from the corresponding type of the
/// conversion function.
// llvm::FoldingSet<detail::CXXSpecialNameExtra> CXXConversionFunctionNames;

/// Manage the uniqued CXXOperatorIdName, which contain extra information
/// for the name of overloaded C++ operators. getCXXOperatorName
/// can be used to obtain a DeclName from the operator kind.
// detail::CXXOperatorIdName CXXOperatorNames[NUM_OVERLOADED_OPERATORS];

/// Manage the uniqued CXXLiteralOperatorIdName, which contain extra
/// information for the name of C++ literal operators.
/// getCXXLiteralOperatorName can be used to obtain a DeclName
/// from the corresponding Identifier.
// llvm::FoldingSet<LiteralOperatorIdName> LiteralOperatorNames;

/// Manage the uniqued CXXDeductionGuideNameExtra, which contain
/// extra information for the name of a C++ deduction guide.
/// getCXXDeductionGuideName can be used to obtain a DeclName
/// from the corresponding template declaration.
// llvm::FoldingSet<detail::CXXDeductionGuideNameExtra>
// CXXDeductionGuideNames;

// public:
//   DeclNameTable(const ASTContext &tc);
//   DeclNameTable(const DeclNameTable &) = delete;
//   DeclNameTable &operator=(const DeclNameTable &) = delete;
//   DeclNameTable(DeclNameTable &&) = delete;
//   DeclNameTable &operator=(DeclNameTable &&) = delete;
//   ~DeclNameTable() = default;

//   /// Create a declaration name that is a simple identifier.
//   DeclName GetIdentifier(const Identifier *identifier) {
//     return DeclName(identifier);
//   }

//   /// Returns the name of a constructor for the given Type.
//   DeclName GetConstructorName(CanType Ty);

//   /// Returns the name of a destructor for the given Type.
//   DeclName GetDestructorName(CanType Ty);

/// Returns the name of a destructor for the given Type.
// DeclName GetUsingName(CanType Ty);

/// Returns the name of a deduction guide for the given template.
// DeclName GDeductionGuideName(GenericDecl *TD);

/// Returns the name of a conversion function for the given Type.
// DeclName GConversionFunctionName(CanType Ty);

/// Returns a declaration name for special kind of C++ name,
/// e.g., for a constructor, destructor, or conversion function.
/// Kind must be one of:
///   * DeclName::ConstructorName,
///   * DeclName::DestructorName or
///   * DeclName::ConversionFunctionName
// TODO: What about using A.B.C;
// DeclName GetSpecialName(DeclNameKind declNameTy, CanType canQualTy);

/// Get the name of the overloadable C++ operator corresponding to Op.
// DeclName getCXXOperatorName(OverloadedOperatorKind Op) {
//  return DeclName(&CXXOperatorNames[Op]);
//}

/// Get the name of the literal operator function with II as the identifier.
// DeclName GetLiteralOperatorName(Identifier *identifier);
//};

// raw_ostream &operator<<(raw_ostream &os, DeclName name);

// /// Ordering on two declaration names. If both names are identifiers,
// /// this provides a lexicographical ordering.
// inline bool operator<(DeclName LHS, DeclName RHS) {
//   return DeclName::Compare(LHS, RHS) < 0;
// }

// /// Ordering on two declaration names. If both names are identifiers,
// /// this provides a lexicographical ordering.
// inline bool operator>(DeclName LHS, DeclName RHS) {
//   return DeclName::Compare(LHS, RHS) > 0;
// }

// /// Ordering on two declaration names. If both names are identifiers,
// /// this provides a lexicographical ordering.
// inline bool operator<=(DeclName LHS, DeclName RHS) {
//   return DeclName::Compare(LHS, RHS) <= 0;
// }

// /// Ordering on two declaration names. If both names are identifiers,
// /// this provides a lexicographical ordering.
// inline bool operator>=(DeclName LHS, DeclName RHS) {
//   return DeclName::Compare(LHS, RHS) >= 0;
// }

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
