#include "stone/Basic/DiagnosticEngine.h"

using stone::CodeFix;
using stone::CodeFixer;
using stone::Diagnostic;
using stone::DiagnosticEngine;
using stone::InFlightDiagnostic;

void Diagnostic::Format(llvm::SmallVectorImpl<char> &outStr,
                        const DiagnosticFormatOptions &fmtOptions) const {

  diag::ArgumentKind kind = diag::ArgumentKind::None;
  switch (kind) {
  case diag::ArgumentKind::STDStr:
    break;
  case diag::ArgumentKind::LLVMStr:
    break;
  case diag::ArgumentKind::SInt:
    break;
  case diag::ArgumentKind::UInt:
    break;
  case diag::ArgumentKind::TokenType:
    break;
  case diag::ArgumentKind::Syntax:
    // Ignore and handle elswhere
    break;
  default:
    // Should not get here
    break;
  }
}

/// Format the given format-string into the output buffer using the
/// arguments stored in this diagnostic.
void Diagnostic::Format(const char *diagStr, const char *diagEnd,
                        llvm::SmallVectorImpl<char> &outStr,
                        const DiagnosticFormatOptions &fmtOptions) const {}

CodeFix::CodeFix(CharSrcRange range, llvm::StringRef code)
    : range(range), code(code) {}

// CodeFix::CodeFix(CharSrcRange R, llvm::StringRef code)
//     : CodeFix(R, code, llvm::ArrayRef<diag::Argument>()) {}

// CodeFix::CodeFix(CodeFixer& fixer, CharSrcRange R, llvm::StringRef code,
//           llvm::ArrayRef<diag::Argument> arguments) :
// range(R), code(code), args(arguments.begin(), arguments.end()){}

llvm::StringRef CodeFixer::GetFixIDString(const FixID fixID) {
  assert(false && "Not implemented");
  return llvm::StringRef();
}

/// Create a code modification hint that inserts the given
/// code string at a specific location.
// InFlightDiagnostic &CodeFixer::InsertFromLoc(SrcLoc insertionLoc,
//                                              llvm::StringRef code,
//                                              bool beforePreviousInsertions) {

//   inFlightDiag.GetDiagEngine().GetCurrentDiagnostic().GetContext().AddFix(
//       CodeFix(CharSrcRange::getCharRange(insertionLoc, insertionLoc),
//               CharSrcRange(), code, beforePreviousInsertions));

//   return inFlightDiag;
// }

/// Create a code modification hint that inserts the given
/// code from \p FromRange at a specific location.
// InFlightDiagnostic &CodeFixer::InsertFromRange(SrcLoc insertionLoc,
//                                                CharSrcRange fromRange,
//                                                bool beforePreviousInsertions)
//                                                {

//   inFlightDiag.GetDiagEngine().GetCurrentDiagnostic().GetContext().AddFix(
//       CodeFix(CharSrcRange::getCharRange(insertionLoc, insertionLoc),
//       fromRange,
//               llvm::StringRef(), beforePreviousInsertions));
//   return inFlightDiag;
// }
/// Create a code modification hint that removes the given
/// source range.
// InFlightDiagnostic &CodeFixer::RemoveRange(CharSrcRange removeRange) {

//   inFlightDiag.GetDiagEngine().GetCurrentDiagnostic().GetContext().AddFix(
//       CodeFix(CharSrcRange(), removeRange, llvm::StringRef()));

//   return inFlightDiag;
// }
// InFlightDiagnostic &CodeFixer::RemoveRange(SrcRange removeRange) {
//   return RemoveRange(CharSrcRange::getTokenRange(removeRange));
// }

/// Create a code modification hint that replaces the given
/// source range with the given code string.
// InFlightDiagnostic &CodeFixer::Replace(CharSrcRange removeRange,
//                                        llvm::StringRef code) {

//   inFlightDiag.GetDiagEngine().GetCurrentDiagnostic().GetContext().AddFix(
//       CodeFix(CharSrcRange(), removeRange, code));

//   return inFlightDiag;
// }

// InFlightDiagnostic &CodeFixer::Replace(SrcRange removeRange,
//                                        llvm::StringRef code) {
//   return Replace(CharSrcRange::getTokenRange(removeRange), code);
// }

// InFlightDiagnostic &CodeFixer::Highlight(SrcRange range) {

//   return inFlightDiag;
// }
// /// Add a character-based range to the currently-active diagnostic.
// InFlightDiagnostic &CodeFixer::HighlightChars(SrcLoc sartLoc, SrcLoc endLoc)
// {

//   return inFlightDiag;
// }

InFlightDiagnostic &CodeFixer::Replace(SrcRange range, StringRef formatString,
                                       ArrayRef<diag::Argument> args) {

  return inFlightDiag;
}

InFlightDiagnostic &CodeFixer::ReplaceChars(SrcLoc start, SrcLoc end,
                                            StringRef formatString,
                                            ArrayRef<diag::Argument> args) {
  return inFlightDiag;
}

InFlightDiagnostic &
CodeFixer::InsertAfter(SrcLoc loc, llvm::StringRef formatString,
                       llvm::ArrayRef<diag::Argument> args) {

  assert(inFlightDiag.tokenable && "InsertAfter requires a Tokenable");
  loc = inFlightDiag.tokenable->GetLocForEndOfToken(
      inFlightDiag.GetDiagEngine().GetSrcMgr(), loc);
  
  return Insert(loc, formatString, args);
}
