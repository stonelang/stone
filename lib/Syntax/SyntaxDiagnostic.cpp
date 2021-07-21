#include "stone/Syntax/SyntaxDiagnosticArgument.h"
#include "stone/Basic/DiagnosticEngine.h"

using stone::SyntaxDiagnostic;

void SyntaxDiagnostic::Format(llvm::SmallVectorImpl<char> &outStr,
                              const DiagnosticFormatOptions &fmtOptions) const {

  diag::SyntaxArgumentKind kind = diag::SyntaxArgumentKind::None;
  switch (kind) {
  case diag::SyntaxArgumentKind::Decl:
    break;
  default:
    // Should not get here
    break;
  }
}

/// Format the given format-string into the output buffer using the
/// arguments stored in this diagnostic.
void SyntaxDiagnostic::Format(const char *diagStr, const char *diagEnd,
                              llvm::SmallVectorImpl<char> &outStr,
                              const DiagnosticFormatOptions &fmtOptions) const {

}

// void DeclDiagnosticArgument::Diagnose(DiagnosticEngine &de) const {

//   // TODO: UB -- this may be too expensive
//   auto arg = static_cast<const syn::Decl *>(val);
//   de.Issue(arg->GetLoc(), 1) << "test";
// }

// void TypeDiagnosticArgument::Diagnose(DiagnosticEngine &de) const {
//   auto arg = static_cast<const syn::Type *>(val);
// }

// void DeclContextDiagnosticArgument::Diagnose(DiagnosticEngine &de) const {
//   auto arg = static_cast<const syn::DeclContext *>(val);
// }
// void IdentifierDiagnosticArgument::Diagnose(DiagnosticEngine &de) const {

//   auto arg = static_cast<const syn::Identifier *>(val);
// }
