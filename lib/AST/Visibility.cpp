#include "stone/AST/Visibility.h"
#include "stone/AST/ASTVisitor.h"

using namespace stone;

// Diagnose declarations whose signatures refer to unavailable types.
class DeclVisibilityChecker : public DeclVisitor<DeclVisibilityChecker> {
  ExportContext Where;

public:
  explicit DeclVisibilityChecker(ExportContext where) : Where(where) {}
};

ExportContext::ExportContext(
    DeclContext *DC, VisibilityContext runningOSVersion,
    std::optional<PlatformKind> unavailablePlatformKind) {}

ExportContext ExportContext::ForDeclSignature(Decl *D) {}

/// At a high level, this checks the given declaration's signature does not
/// reference any other declarations that are less visible than the
/// declaration itself. Related checks may also be performed.
void stone::CheckVisibilityControl(Decl *D) {}

/// Check the QualType visibility level
void stone::CheckVisibilityControl(QualType ty) {}
