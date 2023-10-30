#ifndef STONE_SEM_TYPECHECKAVAILABILITY_H
#define STONE_SEM_TYPECHECKAVAILABILITY_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/AST/AttributeKind.h"
#include "stone/AST/Availability.h"
#include "stone/AST/AvailabilitySpec.h"
#include "stone/AST/DeclContext.h"
#include "stone/AST/Identifier.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"

namespace stone {
class InFlightDiagnostic;
class TypeCheckerOptions;
class TypeCheckerListener;

namespace ast {
class Decl;
class ASTFile;
class Module;
// class ApplyExpr;
class AvailableAttribute;
class Expr;
// class ClosureExpr;
class InterfaceConformanceRef;
class RootInterfaceConformance;
class Stmt;
class SubstitutionMap;
class Type;
// class TypeRepr;
class ValueDecl;

} // namespace ast
namespace sem {
class ExportContext;

enum class DeclAvailabilityFlag : uint8_t {
  /// Do not diagnose uses of protocols in versions before they were introduced.
  /// We allow a type to conform to a protocol that is less available than the
  /// type itself. This enables a type to retroactively model or directly
  /// conform
  /// to a protocol only available on newer OSes and yet still be used on older
  /// OSes.
  AllowPotentiallyUnavailableProtocol = 1 << 0,

  /// Diagnose uses of declarations in versions before they were introduced, but
  /// do not return true to indicate that a diagnostic was emitted.
  ContinueOnPotentialUnavailability = 1 << 1,

  /// If a diagnostic must be emitted, use a variant indicating that the usage
  /// is inout and both the getter and setter must be available.
  ForInout = 1 << 2,

  /// If an error diagnostic would normally be emitted, demote the error to a
  /// warning. Used for ObjC key path components.
  ForObjCKeyPath = 1 << 3,

  /// Do not diagnose potential decl unavailability if that unavailability
  /// would only occur at or below the deployment target.
  AllowPotentiallyUnavailableAtOrBelowDeploymentTarget = 1 << 4,
};
using DeclAvailabilityFlags = OptionSet<DeclAvailabilityFlag>;

// This enum must be kept in astc with
// diag::decl_from_hidden_module and
// diag::conformance_from_implementation_only_module.
enum class ExportabilityReason : unsigned {
  General,
  PropertyWrapper,
  ResultBuilder,
  ExtensionWithPublicMembers,
  ExtensionWithConditionalConformances
};

/// A description of the restrictions on what declarations can be referenced
/// from the signature or body of a declaration.
///
/// We say a declaration is "exported" if all of the following holds:
///
/// - the declaration is `public` or `@usableFromInline`
/// - the declaration is not `@_spi`
/// - the declaration was not imported from an `@_implementationOnly` import
///
/// The "signature" of a declaration is the set of all types written in the
/// declaration (such as function parameter and return types), but not
/// including the function body.
///
/// The signature of an exported declaration can only reference other
/// exported types.
///
/// The body of an inlinable function can only reference other `public` and
/// `@usableFromInline` declarations; furthermore, if the inlinable
/// function is not `@_spi`, its body can only reference other exported
/// declarations.
///
/// The ExportContext also stores if the location in the program is inside
/// of a function or type body with deprecated or unavailable availability.
/// This allows referencing other deprecated and unavailable declarations,
/// without producing a warning or error, respectively.
class ExportContext final {

  ast::DeclContext *DC;
  ast::AvailabilityContext runningOSVersion;
  ast::FragileFunction fragileFunction;

  unsigned SPI : 1;
  unsigned Exported : 1;
  unsigned Deprecated : 1;
  unsigned Implicit : 1;
  unsigned Unavailable : 1;
  unsigned Platform : 8;
  unsigned Reason : 3;

  ExportContext(ast::DeclContext *DC, ast::AvailabilityContext runningOSVersion,
                ast::FragileFunction fragileFunction, bool spi, bool exported,
                bool implicit, bool deprecated,
                llvm::Optional<PlatformKind> unavailablePlatformKind);

public:
  /// Create an instance describing the types that can be referenced from the
  /// given declaration's signature.
  ///
  /// If the declaration is exported, the resulting context is restricted to
  /// referencing exported types only. Otherwise it can reference anything.
  static ExportContext ForDeclSignature(ast::Decl *D);

  /// Create an instance describing the declarations that can be referenced
  /// from the given function's body.
  ///
  /// If the function is inlinable, the resulting context is restricted to
  /// referencing ABI-public declarations only. Furthermore, if the function
  /// is exported, referenced declarations must also be exported. Otherwise
  /// it can reference anything.
  static ExportContext ForFunctionBody(ast::DeclContext *DC, SrcLoc loc);

  /// Create an instance describing associated conformances that can be
  /// referenced from the conformance defined by the given DeclContext,
  /// which must be a NominalTypeDecl or ExtensionDecl.
  static ExportContext ForConformance(ast::DeclContext *DC,
                                      ast::InterfaceDecl *D);

  /// Produce a new context with the same properties as this one, except
  /// changing the ExportabilityReason. This only affects diagnostics.
  ExportContext WithReason(ExportabilityReason reason) const;

  /// Produce a new context with the same properties as this one, except
  /// that if 'exported' is false, the resulting context can reference
  /// declarations that are not exported. If 'exported' is true, the
  /// resulting context is identical to this one.
  ///
  /// That is, this will perform a 'bitwise and' on the 'exported' bit.
  ExportContext WithExported(bool exported) const;

  ast::DeclContext *GetDeclContext() const { return DC; }

  ast::AvailabilityContext GetAvailabilityContext() const {
    return runningOSVersion;
  }

  /// If not 'None', the context has the inlinable function body restriction.
  ast::FragileFunction GetFragileFunction() const { return fragileFunction; }

  /// If true, the context is part of a astthesized declaration, and
  /// availability checking should be disabled.
  bool IsImplicit() const { return Implicit; }

  /// If true, the context is SPI and can reference SPI declarations.
  bool IsSPI() const { return SPI; }

  /// If true, the context is exported and cannot reference SPI declarations
  /// or declarations from `@_implementationOnly` imports.
  bool IsExported() const { return Exported; }

  /// If true, the context is part of a deprecated declaration and can
  /// reference other deprecated declarations without warning.
  bool IsDeprecated() const { return Deprecated; }

  llvm::Optional<PlatformKind> getUnavailablePlatformKind() const;

  /// If true, the context can only reference exported declarations, either
  /// because it is the signature context of an exported declaration, or
  /// because it is the function body context of an inlinable function.
  bool MustOnlyReferenceExportedDecls() const;

  /// Get the ExportabilityReason for diagnostics. If this is 'None', there
  /// are no restrictions on referencing unexported declarations.
  Optional<ExportabilityReason> GetExportabilityReason() const;
};

/// Check if a declaration is exported as part of a module's external interface.
/// This includes public and @usableFromInline decls.
bool IsExported(const ast::ValueDecl *VD);
// bool isExported(const ExtensionDecl *ED);
bool IsExported(const ast::Decl *D);

/// Diagnose uses of unavailable declarations in expressions.
void DiagnoseExprAvailability(const ast::Expr *E, ast::DeclContext *DC);

/// Diagnose uses of unavailable declarations in statements (via patterns, etc)
/// but not expressions, unless \p walkRecursively was specified.
///
/// \param walkRecursively Whether nested statements and expressions should
/// be visited, too.
void DiagnoseStmtAvailability(const ast::Stmt *S, ast::DeclContext *DC,
                              bool walkRecursively = false);

/// Diagnose uses of unavailable conformances in types.
void DiagnoseTypeAvailability(ast::Type T, SrcLoc loc,
                              const ExportContext &context,
                              DeclAvailabilityFlags flags = None);

bool DiagnoseConformanceAvailability(
    SrcLoc loc, ast::InterfaceConformanceRef conformance,
    const ExportContext &context, ast::Type depTy = ast::Type(),
    ast::Type replacementTy = ast::Type(),
    bool useConformanceAvailabilityErrorsOption = false);

bool DiagnoseSubstitutionMapAvailability(
    SrcLoc loc, ast::SubstitutionMap subs, const ExportContext &context,
    ast::Type depTy = ast::Type(), ast::Type replacementTy = ast::Type(),
    bool useConformanceAvailabilityErrorsOption = false,
    bool suppressParameterizationCheckForOptional = false);

/// Diagnose uses of unavailable declarations. Returns true if a diagnostic
/// was emitted.
bool DiagnoseDeclAvailability(const ast::ValueDecl *D, SrcRange R,
                              const ast::Expr *call, const ExportContext &where,
                              DeclAvailabilityFlags flags = None);

void DiagnoseUnavailableOverride(ast::ValueDecl *override,
                                 const ast::ValueDecl *base,
                                 const ast::AvailableAttribute *attr);

/// Emit a diagnostic for references to declarations that have been
/// marked as unavailable, either through "unavailable" or "obsoleted:".
bool DiagnoseExplicitUnavailability(const ast::ValueDecl *D, SrcRange R,
                                    const ExportContext &Where,
                                    const ast::Expr *call,
                                    DeclAvailabilityFlags Flags = None);

/// Emit a diagnostic for references to declarations that have been
/// marked as unavailable, either through "unavailable" or "obsoleted:".
bool DiagnoseExplicitUnavailability(
    const ast::ValueDecl *D, SrcRange R, const ExportContext &Where,
    DeclAvailabilityFlags Flags,
    llvm::function_ref<void(InFlightDiagnostic &)> attachRenameFixIts);

/// Emit a diagnostic for references to declarations that have been
/// marked as unavailable, either through "unavailable" or "obsoleted:".
// bool DiagnoseExplicitUnavailability(
//     SrcLoc loc, const RootProtocolConformance *rootConf,
//     const ExtensionDecl *ext, const ExportContext &where,
//     bool useConformanceAvailabilityErrorsOption = false);

/// Diagnose uses of the runtime features of parameterized protools. Returns
/// \c true if a diagnostic was emitted.
bool DiagnoseParameterizedProtocolAvailability(SrcRange loc,
                                               const ast::DeclContext *DC);

/// Check if \p decl has a introduction version required by
/// -require-explicit-availability
void CheckExplicitAvailability(ast::Decl *decl);

/// Check if \p D needs to be checked for correct availability depending on the
/// flag -check-api-availability-only.
bool ShouldCheckAvailability(const ast::Decl *D);

} // namespace sem
} // namespace stone
#endif
