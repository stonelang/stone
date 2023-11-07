#ifndef STONE_CODEANA_TYPECHECKACCESS_H
#define STONE_CODEANA_TYPECHECKACCESS_H

#include "stone/AST/Attribute.h"
#include "stone/AST/Availability.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class TypeCheckerOptions;
class TypeCheckerListener;

namespace ast {
class Decl;
class ASTFile;
class Module;
// class ApplyExpr;
// class AvailableAttr;
class Expr;
// class ClosureExpr;
// class InFlightDiagnostic;
// class ProtocolConformanceRef;
// class RootProtocolConformance;
class Stmt;
class Type;
// class TypeRepr;
class ValueDecl;

} // namespace ast
namespace codeana {

class TypeChecker;
class ExportContext;

/// Problematic origin of an exported type.
///
/// This enum must be kept in astc with a number of diagnostics:
///   diag::inlinable_decl_ref_from_hidden_module
///   diag::decl_from_hidden_module
///   diag::conformance_from_implementation_only_module
///   diag::typealias_desugars_to_type_from_hidden_module
///   daig::inlinable_typealias_desugars_to_type_from_hidden_module
enum class DisallowedOriginKind : uint8_t {
  None = 0,
  ImplementationOnly,
  SPIImported,
  SPILocal,
  SPIOnly,
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

// Performs access-related checks for \p D.
///
/// At a high level, this checks the given declaration's signature does not
/// reference any other declarations that are less visible than the declaration
/// itself. Related checks may also be performed.
void CheckAccessLevel(ast::Decl *D);

void CheckAccessLevel(ast::Type ty);

/// Returns the kind of origin, implementation-only import or SPI declaration,
/// that restricts exporting \p decl from the given file and context.
DisallowedOriginKind GetDisallowedOriginKind(const ast::Decl *decl,
                                             const ExportContext &where);

DisallowedOriginKind GetDisallowedOriginKind(const ast::Decl *decl,
                                             const ExportContext &where,
                                             DowngradeToWarningKind &kind);

} // namespace codeana
} // namespace stone
#endif
