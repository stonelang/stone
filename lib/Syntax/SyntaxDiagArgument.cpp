#include "stone/Syntax/SyntaxDiagArgument.h"
#include "stone/Basic/DiagnosticEngine.h"

using namespace stone;

void DeclDiagnosticArgument::Diagnose(DiagnosticEngine &de) const {

  // TODO: UB -- this may be too expensive
  auto arg = static_cast<const syn::Decl *>(val);
  de.Issue(arg->GetLoc(), 1) << "test";
}

void TypeDiagnosticArgument::Diagnose(DiagnosticEngine &de) const {
  auto arg = static_cast<const syn::Type *>(val);
}

void DeclContextDiagnosticArgument::Diagnose(DiagnosticEngine &de) const {
  auto arg = static_cast<const syn::DeclContext *>(val);
}
void IdentifierDiagnosticArgument::Diagnose(DiagnosticEngine &de) const {

  auto arg = static_cast<const syn::Identifier *>(val);
}
