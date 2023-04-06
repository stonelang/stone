#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"

using namespace stone;

SyntaxDiagnostic::~SyntaxDiagnostic() {}

// void SyntaxDiagnostic::Format(llvm::SmallVectorImpl<char> &outStr,
//                               const DiagnosticFormatOptions &fmtOptions)
//                               const {
//   diag::SyntaxArgumentKind kind = diag::SyntaxArgumentKind::None;
//   switch (kind) {
//   case diag::SyntaxArgumentKind::Decl:
//     break;
//   default:
//     // Should not get here
//     break;
//   }
// }

/// Format the given format-string into the output buffer using the
/// arguments stored in this diagnostic.
// void SyntaxDiagnostic::Format(const char *diagStr, const char *diagEnd,
//                               llvm::SmallVectorImpl<char> &outStr,
//                               const DiagnosticFormatOptions &fmtOptions)
//                               const {

// }

// void DeclDiagnosticArgument::PrintD(DiagnosticEngine &de) const {

//   // TODO: UB -- this may be too expensive
//   auto arg = static_cast<const syn::Decl *>(val);
//   de.Issue(arg->GetLoc(), 1) << "test";
// }

// void TypeDiagnosticArgument::PrintD(DiagnosticEngine &de) const {
//   auto arg = static_cast<const syn::Type *>(val);
// }

// void DeclContextDiagnosticArgument::PrintD(DiagnosticEngine &de) const {
//   auto arg = static_cast<const syn::DeclContext *>(val);
// }
// void IdentifierDiagnosticArgument::PrintD(DiagnosticEngine &de) const {

//   auto arg = static_cast<const syn::Identifier *>(val);
// }

SyntaxDiagnosticFormatter::SyntaxDiagnosticFormatter() {}

void SyntaxDiagnosticFormatter::Format(ColorStream &out,
                                       DiagnosticDetail &detail,
                                       DiagnosticFormatOptions fmtOpts) {

  // printf("%s\n", "Hello syntax formatting");
}

void SyntaxDiagnosticFormatter::Format(ColorStream &out, llvm::StringRef text,
                                       llvm::ArrayRef<diag::Argument> args,
                                       DiagnosticFormatOptions fmtOpts) {}


void SyntaxDiagnosticFormatter::FormatArgument(ColorStream &out, llvm::StringRef modifier,
                      llvm::StringRef modifierArguments,
                      ArrayRef<diag::Argument> args, unsigned argIndex,
                      DiagnosticFormatOptions fmtOpts) {}



SyntaxDiagnosticEmitter::SyntaxDiagnosticEmitter(SyntaxDiagnosticFormatter &formatter) : TextDiagnosticEmitter(formatter){}

//SyntaxDiagnosticEmitter::SyntaxDiagnosticEmitter() {}

// SyntaxDiagnosticEmitter::EmitDiagnostic() {

// }
