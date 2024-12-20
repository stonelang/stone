#ifndef STONE_SEM_TYPECHECKER_H
#define STONE_SEM_TYPECHECKER_H

#include "stone/AST/Decl.h"
#include "stone/AST/Module.h"

namespace stone {

class Decl;
class SourceFile;
class ModuleDecl;
class Expr;
class Stmt;
class QualType;
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

class TypeChecker final {
  SourceFile &sourceFile;

public:
  TypeChecker(SourceFile &sourceFile);

public:
  bool TypeCheckTopLevelDecls();
  bool TypeCheckTopLevelDecl(Decl *topLevelDecl);

public:
  // Performs access-related checks for \p D.
  ///
  /// At a high level, this checks the given declaration's signature does not
  /// reference any other declarations that are less visible than the
  /// declaration itself. Related checks may also be performed.
  void TypeCheckVisibilityLevel(Decl *D);

  void TypeCheckVisibilityLevel(QualType ty);
};

} // namespace stone

#endif