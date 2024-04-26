#include "stone/Support/DiagnosticEngine.h"
#include "stone/Support/LexerBase.h"

using namespace stone;

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

DiagnosticFixIt::DiagnosticFixIt(CharSrcRange range, llvm::StringRef code)
    : range(range), code(code) {}

// DiagnosticFixIt::DiagnosticFixIt(CharSrcRange R, llvm::StringRef code)
//     : DiagnosticFixIt(R, code, llvm::ArrayRef<DiagnosticArgument>()) {}

// DiagnosticFixIt::DiagnosticFixIt(InFlightDiagnostic& fixer, CharSrcRange R,
// llvm::StringRef code,
//           llvm::ArrayRef<DiagnosticArgument> arguments) :
// range(R), code(code), args(arguments.begin(), arguments.end()){}

llvm::StringRef InFlightDiagnostic::GetFixItIDString(const FixItID fixItID) {
  assert(false && "Not implemented");
  return llvm::StringRef();
}

/// Create a code modification hint that inserts the given
/// code string at a specific location.
// InFlightDiagnostic &InFlightDiagnostic::InsertFromLoc(SrcLoc insertionLoc,
//                                              llvm::StringRef code,
//                                              bool beforePreviousInsertions) {

//   inFlightDiag.GetDiags().GetCurrentDiagnostic().GetLangContext().AddFix(
//       DiagnosticFixIt(CharSrcRange::getCharRange(insertionLoc, insertionLoc),
//               CharSrcRange(), code, beforePreviousInsertions));

//   return inFlightDiag;
// }

/// Create a code modification hint that inserts the given
/// code from \p FromRange at a specific location.
// InFlightDiagnostic &InFlightDiagnostic::InsertFromRange(SrcLoc insertionLoc,
//                                                CharSrcRange fromRange,
//                                                bool beforePreviousInsertions)
//                                                {

//   inFlightDiag.GetDiags().GetCurrentDiagnostic().GetLangContext().AddFix(
//       DiagnosticFixIt(CharSrcRange::getCharRange(insertionLoc, insertionLoc),
//       fromRange,
//               llvm::StringRef(), beforePreviousInsertions));
//   return inFlightDiag;
// }
/// Create a code modification hint that removes the given
/// source range.
// InFlightDiagnostic &InFlightDiagnostic::RemoveRange(CharSrcRange removeRange)
// {

//   inFlightDiag.GetDiags().GetCurrentDiagnostic().GetLangContext().AddFix(
//       DiagnosticFixIt(CharSrcRange(), removeRange, llvm::StringRef()));

//   return inFlightDiag;
// }
// InFlightDiagnostic &InFlightDiagnostic::RemoveRange(SrcRange removeRange) {
//   return RemoveRange(CharSrcRange::getTokenRange(removeRange));
// }

/// Create a code modification hint that replaces the given
/// source range with the given code string.
// InFlightDiagnostic &InFlightDiagnostic::Replace(CharSrcRange removeRange,
//                                        llvm::StringRef code) {

//   inFlightDiag.GetDiags().GetCurrentDiagnostic().GetLangContext().AddFix(
//       DiagnosticFixIt(CharSrcRange(), removeRange, code));

//   return inFlightDiag;
// }

// InFlightDiagnostic &InFlightDiagnostic::Replace(SrcRange removeRange,
//                                        llvm::StringRef code) {
//   return Replace(CharSrcRange::getTokenRange(removeRange), code);
// }

// InFlightDiagnostic &InFlightDiagnostic::Highlight(SrcRange range) {

//   return inFlightDiag;
// }
// /// Add a character-based range to the currently-active diagnostic.
// InFlightDiagnostic &InFlightDiagnostic::HighlightChars(SrcLoc sartLoc, SrcLoc
// endLoc)
// {

//   return inFlightDiag;
// }

/// Add a token-based replacement fix-it to the currently-active
/// diagnostic.
InFlightDiagnostic &InFlightDiagnostic::Replace(SrcRange range,
                                                StringRef formatString) {
  return *this;
}

InFlightDiagnostic &
InFlightDiagnostic::Replace(SrcRange range, StringRef formatString,
                            ArrayRef<DiagnosticArgument> args) {
  return *this;
}

InFlightDiagnostic &
InFlightDiagnostic::ReplaceChars(SrcLoc start, SrcLoc end,
                                 StringRef formatString,
                                 ArrayRef<DiagnosticArgument> args) {
  return *this;
}

InFlightDiagnostic &
InFlightDiagnostic::InsertAfter(SrcLoc loc, llvm::StringRef formatString,
                                llvm::ArrayRef<DiagnosticArgument> args) {

  assert(GetDiags()->GetLexerBase() && "InsertAfter requires a Tokenable");
  loc = GetDiags()->GetLexerBase()->GetLocForEndOfToken(GetDiags()->GetSrcMgr(),
                                                        loc);
  return Insert(loc, formatString, args);
}
