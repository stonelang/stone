#ifndef STONE_SYNTAX_DECLNAME_H
#define STONE_SYNTAX_DECLNAME_H

#include "stone/Basic/Dumpable.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/OperatorKind.h"
#include "stone/Basic/Printable.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Type.h"
#include "llvm/ADT/FoldingSet.h"

namespace stone {
class PrintingPolicy;
namespace syn {

class SyntaxContext;
class DeclName;
class DeclNameTable;

class TemplateDecl;
// class TypeSourceInfo
class UseDecl;

enum class DeclNameKind : uint8_t {
  Basic = 0,
  Constructor,
  Destructor,
  Operator,
  LiteralOperator,
  UsingDirective,
};

// class DeclNameBase {
// private:
//   /// As above, for special constructor DeclNames.
//   static const Identifier::Aligner ConstructorIdentifierData;
//   /// As above, for special destructor DeclNames.
//   static const Identifier::Aligner DestructorIdentifierData;

//   static const Identifier::Aligner OperatorIdentifierData;

//   static const Identifier::Aligner LiteraldentifierData;

//   static const Identifier::Aligner UsingIdentifierData;

//   Identifier identifier;

// public:
//   DeclNameBase() : DeclNameBase(Identifier()) {}

//   DeclNameBase(Identifier identifier) : identifier(identifier) {}
// };

namespace detail {
// Using, Constructor, and Destructor
class alignas(IdentifierAlignment) SpecialDeclNameExtra
    : public llvm::FoldingSetNode {
  friend class DeclName;
  friend class DeclNameTable;

  /// The type associated with this declaration name.
  QualType ty;

public:
  SpecialDeclNameExtra(QualType inputTy) : ty(inputTy) {}
  void Profile(llvm::FoldingSetNodeID &ID) {
    // TODO: ID.AddPointer(ty.GetAsOpaquePtr());
  }
};

/// Contains extra information for the name of an overloaded operator
/// in C++, such as "operator+. This do not includes literal or conversion
/// operators. For literal operators see LiteralOperatorIdName and for
/// conversion operators see CXXSpecialNameExtra.
class alignas(IdentifierAlignment) OperatorIdName
    : public detail::SpecialDeclName {
  friend class DeclName;
  friend class DeclNameTable;

  /// The kind of this operator.
  opr::OverloadedOperatorKind Kind = opr::None;
};

/// Contains the actual identifier that makes up the
/// name of a C++ literal operator.
class alignas(IdentifierAlignment) LiteralOperatorIdName
    : public detail::SpecialDeclName,
      public llvm::FoldingSetNode {
  friend class DeclName;
  friend class DeclNameTable;

  Identifier *ID;

  LiteralOperatorIdName(Identifier *II)
      : SpecialDeclName(LiteralOperatorName), ID(II) {}

public:
  void Profile(llvm::FoldingSetNodeID &FSID) { FSID.AddPointer(ID); }
};

} // namespace detail

class DeclName {
  friend class NamedDecl;
  friend class SyntaxContext;

  DeclNameKind declNameKind;

  struct StoredNameKind {
    enum Flags : unsigned {
      Identifier = 0,
      ConstructorName = 1,
      DestructorName = 2,
      ConversionFunctionName = 3,
      OperatorName = 4,
      DeclNameExtra = 5, // TODO: Think about
      PtrMask = 6,
      UncommonNameKindOffset = 7,
    };
  };
  UIntPtr opaquePtr = 0;

  unsigned GetStoredNameKind() const {
    return static_cast<StoredNameKind::Flags>(opaquePtr &
                                              StoredNameKind::PtrMask);
  }
  void *GetPtr() const {
    return reinterpret_cast<void *>(opaquePtr & ~StoredNameKind::PtrMask);
  }

  void SetPtrAndKind(const void *anyPtr, StoredNameKind::Flags kind) {
    uintptr_t intPtr = reinterpret_cast<uintptr_t>(anyPtr);

    assert((kind & ~StoredNameKind::PtrMask) == 0 &&
           "Invalid StoredNameKind in SetPtrAndKind!");

    assert((intPtr & StoredNameKind::PtrMask) == 0 &&
           "Improperly aligned pointer in setPtrAndKind!");

    opaquePtr = intPtr | kind;
  }

public:
  /// Construct a declaration name from an Identifier *.
  DeclName(const Identifier *identifier) {
    SetPtrAndKind(identifier, StoredNameKind::Identifier);
  }

public:
  // TODO:
  bool IsBasic() const { return declNameKind == DeclNameKind::Basic; }
  bool IsConstructor() const {
    return declNameKind == DeclNameKind::Constructor;
  }
  bool IsDestructor() const { return declNameKind == DeclNameKind::Destructor; }
  bool IsOperator() const { return declNameKind == DeclNameKind::Operator; }
  bool IsLiteralOperator() const {
    return declNameKind == DeclNameKind::LiteralOperator;
  }
  bool IsUsingDirective() const {
    return declNameKind == DeclNameKind::UsingDirective;
  }

  // TODO:
  Identifier *GetAsIdentifier() const {}
  DeclNameKind GetDeclNameKind() { return declNameKind; }

public:
  void Print(ColorfulStream &os, const PrintingPolicy *policy = nullptr) const;

  void Dump() const;

public:
  static int Compare(DeclName LHS, DeclName RHS);
};

class DeclNameLoc {
  DeclName name;

public:
public:
  DeclNameLoc(DeclName name) : name(name) {}
};

struct DeclNameInfo final {
private:
  /// Name - The declaration name, also encoding name kind.
  DeclName name;

  /// Loc - The main source location for the declaration name.
  SrcLoc nameLoc;

  /// Info - Further source/type location info for special kinds of names.
  DeclNameLoc specialNameLoc;

public:
  DeclNameInfo() : name(nullptr), nameLoc(SrcLoc()), specialNameLoc(nullptr) {}

  DeclNameInfo(DeclName name, SrcLoc nameLoc)
      : name(name), nameLoc(nameLoc), specialNameLoc(name) {}

  DeclNameInfo(DeclName name, SrcLoc nameLoc, DeclNameLoc specialNameLoc)
      : name(name), nameLoc(nameLoc), specialNameLoc(specialNameLoc) {}

  /// getName - Returns the embedded declaration name.
  DeclName GetName() const { return name; }

  /// setName - Sets the embedded declaration name.
  void SetName(DeclName n) { name = n; }

  /// getLoc - Returns the main location of the declaration name.
  SrcLoc GetNameLoc() const { return nameLoc; }

  /// setLoc - Sets the main location of the declaration name.
  void SetNameLoc(SrcLoc loc) { nameLoc = loc; }

  const DeclNameLoc &GetSpecialNameLoc() const { return specialNameLoc; }
  void SetSpecialNameLoc(const DeclNameLoc &info) { specialNameLoc = info; }

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

private:
  // SrcLoc getEndLocPrivate() const;
};

/// DeclNameTable is used to store and retrieve DeclName
/// instances for the various kinds of declaration names, e.g., normal
/// identifiers, constructor names, etc. This class contains
/// uniqued versions of each of the special names, which can be
/// retrieved using its member functions (e.g., GetConstructorName).
class DeclNameTable final {
  /// Used to allocate elements in the FoldingSets below.
  const SyntaxContext &tc;

  /// Manage the uniqued SpecialDeclName representing stone constructors.
  /// GetConstructorName and GetSpecialName can be used to obtain
  /// a DeclName from the corresponding type of the constructor.
  llvm::FoldingSet<detail::SpecialDeclNameExtra> constructorNames;

  /// Manage the uniqued CXXSpecialNameExtra representing C++ destructors.
  /// getCXXDestructorName and getCXXSpecialName can be used to obtain
  /// a DeclName from the corresponding type of the destructor.
  llvm::FoldingSet<detail::SpecialDeclNameExtra> destructorNames;

  //
  llvm::FoldingSet<detail::SpecialDeclNameExtra> usingNames;

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

public:
  DeclNameTable(const SyntaxContext &tc);
  DeclNameTable(const DeclNameTable &) = delete;
  DeclNameTable &operator=(const DeclNameTable &) = delete;
  DeclNameTable(DeclNameTable &&) = delete;
  DeclNameTable &operator=(DeclNameTable &&) = delete;
  ~DeclNameTable() = default;

  /// Create a declaration name that is a simple identifier.
  DeclName GetIdentifier(const Identifier *identifier) {
    return DeclName(identifier);
  }

  /// Returns the name of a constructor for the given Type.
  DeclName GetConstructorName(CanQualType Ty);

  /// Returns the name of a destructor for the given Type.
  DeclName GetDestructorName(CanQualType Ty);

  /// Returns the name of a destructor for the given Type.
  // DeclName GetUsingName(CanQualType Ty);

  /// Returns the name of a deduction guide for the given template.
  // DeclName GDeductionGuideName(TemplateDecl *TD);

  /// Returns the name of a conversion function for the given Type.
  // DeclName GConversionFunctionName(CanQualType Ty);

  /// Returns a declaration name for special kind of C++ name,
  /// e.g., for a constructor, destructor, or conversion function.
  /// Kind must be one of:
  ///   * DeclName::ConstructorName,
  ///   * DeclName::DestructorName or
  ///   * DeclName::ConversionFunctionName
  // TODO: What about using A.B.C;
  DeclName GetSpecialName(DeclNameKind declNameTy, CanQualType canQualTy);

  /// Get the name of the overloadable C++ operator corresponding to Op.
  // DeclName getCXXOperatorName(OverloadedOperatorKind Op) {
  //  return DeclName(&CXXOperatorNames[Op]);
  //}

  /// Get the name of the literal operator function with II as the identifier.
  // DeclName GetLiteralOperatorName(Identifier *identifier);
};

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
