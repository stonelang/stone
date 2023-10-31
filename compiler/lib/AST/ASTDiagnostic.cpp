#include "stone/AST/ASTDiagnosticArgument.h"
#include "stone/Diag/DiagnosticEngine.h"

using namespace stone;

ASTDiagnostic::~ASTDiagnostic() {}

// void ASTDiagnostic::Format(llvm::SmallVectorImpl<char> &outStr,
//                               const DiagnosticFormatOptions &fmtOptions)
//                               const {
//   diag::ASTArgumentKind kind = diag::ASTArgumentKind::None;
//   switch (kind) {
//   case diag::ASTArgumentKind::Decl:
//     break;
//   default:
//     // Should not get here
//     break;
//   }
// }

/// Format the given format-string into the output buffer using the
/// arguments stored in this diagnostic.
// void ASTDiagnostic::Format(const char *diagStr, const char *diagEnd,
//                               llvm::SmallVectorImpl<char> &outStr,
//                               const DiagnosticFormatOptions &fmtOptions)
//                               const {

// }

// void DeclDiagnosticArgument::PrintD(DiagnosticEngine &de) const {

//   // TODO: UB -- this may be too expensive
//   auto arg = static_cast<const ast::Decl *>(val);
//   de.Issue(arg->GetLoc(), 1) << "test";
// }

// void TypeDiagnosticArgument::PrintD(DiagnosticEngine &de) const {
//   auto arg = static_cast<const ast::Type *>(val);
// }

// void DeclContextDiagnosticArgument::PrintD(DiagnosticEngine &de) const {
//   auto arg = static_cast<const ast::DeclContext *>(val);
// }
// void IdentifierDiagnosticArgument::PrintD(DiagnosticEngine &de) const {

//   auto arg = static_cast<const ast::Identifier *>(val);
// }

ASTDiagnosticFormatter::ASTDiagnosticFormatter() {}

void ASTDiagnosticFormatter::Format(ColorStream &out, Diagnostic &detail,
                                    DiagnosticFormatOptions fmtOpts) {

  // printf("%s\n", "Hello asttax formatting");
}

void ASTDiagnosticFormatter::Format(ColorStream &out, llvm::StringRef text,
                                    llvm::ArrayRef<diag::Argument> args,
                                    DiagnosticFormatOptions fmtOpts) {}

void ASTDiagnosticFormatter::FormatArgument(ColorStream &out,
                                            llvm::StringRef modifier,
                                            llvm::StringRef modifierArguments,
                                            ArrayRef<diag::Argument> args,
                                            unsigned argIndex,
                                            DiagnosticFormatOptions fmtOpts) {}

ASTDiagnosticEmitter::ASTDiagnosticEmitter(ASTDiagnosticFormatter &formatter)
    : TextDiagnosticEmitter(formatter) {}

// ASTDiagnosticEmitter::ASTDiagnosticEmitter() {}

// ASTDiagnosticEmitter::EmitDiagnostic() {

// }
