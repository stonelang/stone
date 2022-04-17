#ifndef STONE_SYNTAX_DECLNAME_H
#define STONE_SYNTAX_DECLNAME_H

#include "stone/Basic/Dumpable.h"
#include "stone/Basic/LLVM.h"
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

template <typename> class CanQual;
using CanQualType = CanQual<syn::Type>;

class DeclNameLoc {
public:
};

enum class DeclNameKind : uint8_t {
  Basic=0,
  Constructor,
  Destructor,
  Operator,
  LiteralOperator,
  UsingDirective,
};
class DeclName : public Dumpable, public Printable {
  friend class NamedDecl;
  friend class SyntaxContext;

  DeclNameKind declNameKind;

public:
  /// Construct a declaration name from an Identifier *.
  DeclName(const Identifier identifier) {
    // TODO: SetPtrAndType(identifier, storedIdentifier);
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
  void Print(ColorfulStream &os,
             const PrintingPolicy *policy = nullptr) const override;

  void Dump() const override;

public:
  static int Compare(DeclName LHS, DeclName RHS);
};

// Using, Constructor, and Destructor
class SpecialDeclName : public DeclName {
public:
};

/// DeclNameTable is used to store and retrieve DeclName
/// instances for the various kinds of declaration names, e.g., normal
/// identifiers, constructor names, etc. This class contains
/// uniqued versions of each of the special names, which can be
/// retrieved using its member functions (e.g., GetConstructorName).
class DeclNameTable {
  /// Used to allocate elements in the FoldingSets below.
  const SyntaxContext &tc;

  /// Manage the uniqued SpecialDeclName representing stone constructors.
  /// GetConstructorName and GetSpecialName can be used to obtain
  /// a DeclName from the corresponding type of the constructor.
  llvm::FoldingSet<SpecialDeclName> constructorNames;

  /// Manage the uniqued CXXSpecialNameExtra representing C++ destructors.
  /// getCXXDestructorName and getCXXSpecialName can be used to obtain
  /// a DeclName from the corresponding type of the destructor.
  llvm::FoldingSet<SpecialDeclName> destructorNames;

  //
  llvm::FoldingSet<SpecialDeclName> usingNames;

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
  DeclName GetIdentifier(const Identifier identifier) {
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
