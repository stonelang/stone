#include "stone/Support/DiagnosticEngine.h"
#include "stone/Support/LexerBase.h"

using stone::Diagnostic;
using stone::DiagnosticEngine;
using stone::DiagnosticFix;
using stone::DiagnosticFixer;
using stone::InFlightDiagnostic;

// void Diagnostic::Format(llvm::SmallVectorImpl<char> &outStr,
//                         const DiagnosticFormatOptions &fmtOptions) const {
//   DiagnosticArgumentKind kind = DiagnosticArgumentKind::None;
//   switch (kind) {
//   case DiagnosticArgumentKind::STDStr:
//     break;
//   case DiagnosticArgumentKind::LLVMStr:
//     break;
//   case DiagnosticArgumentKind::SInt:
//     break;
//   case DiagnosticArgumentKind::UInt:
//     break;
//   case DiagnosticArgumentKind::TokenType:
//     break;
//   case DiagnosticArgumentKind::Syntax:
//     // Ignore and handle elswhere
//     break;
//   default:
//     // Should not get here
//     break;
//   }
// }

/// Format the given format-string into the output buffer using the
/// arguments stored in this diagnostic.
// void Diagnostic::Format(const char *diagStr, const char *diagEnd,
//                         llvm::SmallVectorImpl<char> &outStr,
//                         const DiagnosticFormatOptions &fmtOptions) const {}

DiagnosticFix::DiagnosticFix(CharSrcRange range, llvm::StringRef code)
    : range(range), code(code) {}

// DiagnosticFix::DiagnosticFix(CharSrcRange R, llvm::StringRef code)
//     : DiagnosticFix(R, code, llvm::ArrayRef<DiagnosticArgument>()) {}

// DiagnosticFix::DiagnosticFix(DiagnosticFixer& fixer, CharSrcRange R,
// llvm::StringRef code,
//           llvm::ArrayRef<DiagnosticArgument> arguments) :
// range(R), code(code), args(arguments.begin(), arguments.end()){}

llvm::StringRef DiagnosticFixer::GetFixIDString(const FixID fixID) {
  assert(false && "Not implemented");
  return llvm::StringRef();
}

/// Create a code modification hint that inserts the given
/// code string at a specific location.
// InFlightDiagnostic &DiagnosticFixer::InsertFromLoc(SrcLoc insertionLoc,
//                                              llvm::StringRef code,
//                                              bool beforePreviousInsertions) {

//   inFlightDiag.GetDiags().GetCurrentDiagnostic().GetLangContext().AddFix(
//       DiagnosticFix(CharSrcRange::getCharRange(insertionLoc, insertionLoc),
//               CharSrcRange(), code, beforePreviousInsertions));

//   return inFlightDiag;
// }

/// Create a code modification hint that inserts the given
/// code from \p FromRange at a specific location.
// InFlightDiagnostic &DiagnosticFixer::InsertFromRange(SrcLoc insertionLoc,
//                                                CharSrcRange fromRange,
//                                                bool beforePreviousInsertions)
//                                                {

//   inFlightDiag.GetDiags().GetCurrentDiagnostic().GetLangContext().AddFix(
//       DiagnosticFix(CharSrcRange::getCharRange(insertionLoc, insertionLoc),
//       fromRange,
//               llvm::StringRef(), beforePreviousInsertions));
//   return inFlightDiag;
// }
/// Create a code modification hint that removes the given
/// source range.
// InFlightDiagnostic &DiagnosticFixer::RemoveRange(CharSrcRange removeRange) {

//   inFlightDiag.GetDiags().GetCurrentDiagnostic().GetLangContext().AddFix(
//       DiagnosticFix(CharSrcRange(), removeRange, llvm::StringRef()));

//   return inFlightDiag;
// }
// InFlightDiagnostic &DiagnosticFixer::RemoveRange(SrcRange removeRange) {
//   return RemoveRange(CharSrcRange::getTokenRange(removeRange));
// }

/// Create a code modification hint that replaces the given
/// source range with the given code string.
// InFlightDiagnostic &DiagnosticFixer::Replace(CharSrcRange removeRange,
//                                        llvm::StringRef code) {

//   inFlightDiag.GetDiags().GetCurrentDiagnostic().GetLangContext().AddFix(
//       DiagnosticFix(CharSrcRange(), removeRange, code));

//   return inFlightDiag;
// }

// InFlightDiagnostic &DiagnosticFixer::Replace(SrcRange removeRange,
//                                        llvm::StringRef code) {
//   return Replace(CharSrcRange::getTokenRange(removeRange), code);
// }

// InFlightDiagnostic &DiagnosticFixer::Highlight(SrcRange range) {

//   return inFlightDiag;
// }
// /// Add a character-based range to the currently-active diagnostic.
// InFlightDiagnostic &DiagnosticFixer::HighlightChars(SrcLoc sartLoc, SrcLoc
// endLoc)
// {

//   return inFlightDiag;
// }

/// Add a token-based replacement fix-it to the currently-active
/// diagnostic.
InFlightDiagnostic &DiagnosticFixer::Replace(SrcRange range,
                                             StringRef formatString) {
  return inFlightDiag;
}

InFlightDiagnostic &
DiagnosticFixer::Replace(SrcRange range, StringRef formatString,
                         ArrayRef<DiagnosticArgument> args) {
  return inFlightDiag;
}

InFlightDiagnostic &
DiagnosticFixer::ReplaceChars(SrcLoc start, SrcLoc end, StringRef formatString,
                              ArrayRef<DiagnosticArgument> args) {
  return inFlightDiag;
}

InFlightDiagnostic &
DiagnosticFixer::InsertAfter(SrcLoc loc, llvm::StringRef formatString,
                             llvm::ArrayRef<DiagnosticArgument> args) {
  assert(inFlightDiag.lexerBase && "InsertAfter requires a Tokenable");

  loc = inFlightDiag.lexerBase->GetLocForEndOfToken(
      inFlightDiag.GetDiags().GetSrcMgr(), loc);

  return Insert(loc, formatString, args);
}
