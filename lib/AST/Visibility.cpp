#include "stone/AST/Visibility.h"

using namespace stone;

ExporContext::ExportContext(
    DeclContext *DC, VisibilityContext runningOSVersion,
    std::optional<PlatformKind> unavailablePlatformKind) {}

ExportContext ExporContext::ForDeclSignature(Decl *D) {}
