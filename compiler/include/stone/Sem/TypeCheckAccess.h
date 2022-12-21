#ifndef STONE_SEM_TYPECHECKACCESS_H
#define STONE_SEM_TYPECHECKACCESS_H

#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/Availability.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class TypeCheckerOptions;
class TypeCheckerListener;

namespace syn {
class Decl;
class SyntaxFile;
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

} // namespace syn
namespace sem {
class ExportContext;

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
void CheckAccessLevel(syn::Decl *D);

/// Returns the kind of origin, implementation-only import or SPI declaration,
/// that restricts exporting \p decl from the given file and context.
DisallowedOriginKind GetDisallowedOriginKind(const syn::Decl *decl,
                                             const ExportContext &where);

DisallowedOriginKind GetDisallowedOriginKind(const syn::Decl *decl,
                                             const ExportContext &where,
                                             DowngradeToWarningKind &kind);

} // namespace sem
} // namespace stone
#endif
