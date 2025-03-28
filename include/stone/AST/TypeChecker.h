#ifndef STONE_AST_TYPECHECKER_H
#define STONE_AST_TYPECHECKER_H

#include "stone/AST/Decl.h"
#include "stone/AST/Module.h"
#include "stone/AST/TypeCheckerOptions.h"
namespace stone {

class Decl;
class SourceFile;
class ModuleDecl;
class Expr;
class Stmt;
class Type;
class ValueDecl;

/// Problematic origin of an exported type.
///
/// This enum must be kept in sync with a number of diagnostics:
///   diag::inlinable_decl_ref_from_hidden_module
///   diag::decl_from_hidden_module
///   diag::conformance_from_implementation_only_module
///   diag::typealias_desugars_to_type_from_hidden_module
///   daig::inlinable_typealias_desugars_to_type_from_hidden_module
enum class DisallowedOriginKind : uint8_t {
  None = 0,
  ImplementationOnly,
  MissingImport,

};

/// A uniquely-typed boolean to reduce the chances of accidentally inverting
/// a check.
///
/// \see checkTypeAccess
enum class DowngradeToWarningKind : bool {
  Disable,
  Enabled,
};

class TypeChecker final {
public:
  static bool CheckDecl(Decl *D);

public:
  // Performs access-related checks for \p D.
  ///

  /// Returns the kind of origin, implementation-only import or SPI declaration,
  /// that restricts exporting \p decl from the given file and context.
  // DisallowedOriginKind GetDisallowedOriginKind(const Decl *decl,
  //                                              const ExportContext &where);

  // DisallowedOriginKind GetDisallowedOriginKind(const Decl *decl,
  //                                              const ExportContext &where,
  //                                              DowngradeToWarningKind &kind);

public:
  /// Determine whether one type is a subtype of another.
  ///
  /// \param t1 The potential subtype.
  /// \param t2 The potential supertype.
  /// \param dc The context of the check.
  ///
  /// \returns true if \c t1 is a subtype of \c t2.
  bool IsSubTypeOf(Type t1, Type t2, DeclContext *dc);

public:
  static void CheckSourceFile(SourceFile &sourceFile);
};

} // namespace stone

#endif