#include "stone/Basic/DiagnosticEngine.h"

using stone::CodeFix;
using stone::CodeFixer;
using stone::Diagnostic;
using stone::DiagnosticEngine;
using stone::InFlightDiagnostic;

void Diagnostic::Format(llvm::SmallVectorImpl<char> &outStr,
                        const DiagnosticFormatOptions &fmtOptions) const {

  DiagnosticArgumentType argTy = DiagnosticArgumentType::None;
  switch (argTy) {
  case DiagnosticArgumentType::STDStr:
    break;
  case DiagnosticArgumentType::LLVMStr:
    break;
  case DiagnosticArgumentType::SInt:
    break;
  case DiagnosticArgumentType::UInt:
    break;
  case DiagnosticArgumentType::TokenType:
    break;
  case DiagnosticArgumentType::Complex:
    break;
  default:
    break;
  }
}

/// Format the given format-string into the output buffer using the
/// arguments stored in this diagnostic.
void Diagnostic::Format(const char *diagStr, const char *diagEnd,
                        llvm::SmallVectorImpl<char> &outStr,
                        const DiagnosticFormatOptions &fmtOptions) const {}

CodeFix::CodeFix(CharSrcRange removeRange, CharSrcRange insertFromRange,
                 llvm::StringRef code, bool beforePreviousInsertions)
    : removeRange(removeRange), insertFromRange(insertFromRange), code(code),
      beforePreviousInsertions(beforePreviousInsertions) {}

/// Create a code modification hint that inserts the given
/// code string at a specific location.
InFlightDiagnostic CodeFixer::InsertFromLoc(SrcLoc insertionLoc,
                                            llvm::StringRef code,
                                            bool beforePreviousInsertions) {

  inFlightDiag.GetDiagEngine()->GetCurrentDiagnostic().GetProfile().AddFix(
      CodeFix(CharSrcRange::getCharRange(insertionLoc, insertionLoc),
              CharSrcRange(), code, beforePreviousInsertions));

  return InFlightDiagnostic(inFlightDiag.GetDiagEngine());
}

/// Create a code modification hint that inserts the given
/// code from \p FromRange at a specific location.
InFlightDiagnostic CodeFixer::InsertFromRange(SrcLoc insertionLoc,
                                              CharSrcRange fromRange,
                                              bool beforePreviousInsertions) {

  inFlightDiag.GetDiagEngine()->GetCurrentDiagnostic().GetProfile().AddFix(
      CodeFix(CharSrcRange::getCharRange(insertionLoc, insertionLoc), fromRange,
              llvm::StringRef(), beforePreviousInsertions));

  return InFlightDiagnostic(inFlightDiag.GetDiagEngine());
}
/// Create a code modification hint that removes the given
/// source range.
InFlightDiagnostic CodeFixer::Remove(CharSrcRange removeRange) {

  inFlightDiag.GetDiagEngine()->GetCurrentDiagnostic().GetProfile().AddFix(
      CodeFix(CharSrcRange(), removeRange, llvm::StringRef()));

  return InFlightDiagnostic(inFlightDiag.GetDiagEngine());
}
InFlightDiagnostic CodeFixer::Remove(SrcRange removeRange) {
  return Remove(CharSrcRange::getTokenRange(removeRange));
}

/// Create a code modification hint that replaces the given
/// source range with the given code string.
InFlightDiagnostic CodeFixer::Replace(CharSrcRange removeRange,
                                      llvm::StringRef code) {

  inFlightDiag.GetDiagEngine()->GetCurrentDiagnostic().GetProfile().AddFix(
      CodeFix(CharSrcRange(), removeRange, code));

  return InFlightDiagnostic(inFlightDiag.GetDiagEngine());
}

InFlightDiagnostic CodeFixer::Replace(SrcRange removeRange,
                                      llvm::StringRef code) {
  return Replace(CharSrcRange::getTokenRange(removeRange), code);
}

InFlightDiagnostic CodeFixer::Highlight(SrcRange range) {

  return InFlightDiagnostic(inFlightDiag.GetDiagEngine());
}
/// Add a character-based range to the currently-active diagnostic.
InFlightDiagnostic CodeFixer::HighlightChars(SrcLoc sartLoc, SrcLoc endLoc) {

  return InFlightDiagnostic(inFlightDiag.GetDiagEngine());
}
