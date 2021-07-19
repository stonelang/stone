#include "stone/Basic/DiagnosticEngine.h"

using stone::CodeFix;
using stone::CodeFixer;
using stone::Diagnostic;
using stone::DiagnosticEngine;
using stone::InflightDiagnostic;

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
InflightDiagnostic CodeFixer::InsertFromLoc(SrcLoc insertionLoc,
                                            llvm::StringRef code,
                                            bool beforePreviousInsertions) {

  assert(inflightDiag &&
         "CodeFixer can only be invoked by an InflightDiagnostic");

  inflightDiag->GetCurrentDiagnostic().GetProfile().AddFix(
      CodeFix(CharSrcRange::getCharRange(insertionLoc, insertionLoc),
              CharSrcRange(), code, beforePreviousInsertions));

  return InflightDiagnostic(inflightDiag->GetDiagEngine());
}

/// Create a code modification hint that inserts the given
/// code from \p FromRange at a specific location.
InflightDiagnostic CodeFixer::InsertFromRange(SrcLoc insertionLoc,
                                              CharSrcRange fromRange,
                                              bool beforePreviousInsertions) {

  assert(inflightDiag &&
         "CodeFixer can only be invoked by an InflightDiagnostic");

  inflightDiag->GetCurrentDiagnostic().GetProfile().AddFix(
      CodeFix(CharSrcRange::getCharRange(insertionLoc, insertionLoc), fromRange,
              llvm::StringRef(), beforePreviousInsertions));

  return InflightDiagnostic(inflightDiag->GetDiagEngine());
}
/// Create a code modification hint that removes the given
/// source range.
InflightDiagnostic CodeFixer::Remove(CharSrcRange removeRange) {

  assert(inflightDiag &&
         "CodeFixer can only be invoked by an InflightDiagnostic");

  inflightDiag->GetCurrentDiagnostic().GetProfile().AddFix(
      CodeFix(CharSrcRange(), removeRange, llvm::StringRef()));

  return InflightDiagnostic(inflightDiag->GetDiagEngine());
}
InflightDiagnostic CodeFixer::Remove(SrcRange removeRange) {
  return Remove(CharSrcRange::getTokenRange(removeRange));
}

/// Create a code modification hint that replaces the given
/// source range with the given code string.
InflightDiagnostic CodeFixer::Replace(CharSrcRange removeRange,
                                      llvm::StringRef code) {

  assert(inflightDiag &&
         "CodeFixer can only be invoked by an InflightDiagnostic");

  inflightDiag->GetCurrentDiagnostic().GetProfile().AddFix(
      CodeFix(CharSrcRange(), removeRange, code));

  return InflightDiagnostic(inflightDiag->GetDiagEngine());
}

InflightDiagnostic CodeFixer::Replace(SrcRange removeRange,
                                      llvm::StringRef code) {
  return Replace(CharSrcRange::getTokenRange(removeRange), code);
}

InflightDiagnostic CodeFixer::Highlight(SrcRange range) {
  assert(inflightDiag &&
         "CodeFixer can only be invoked by an InflightDiagnostic");

  return InflightDiagnostic(inflightDiag->GetDiagEngine());
}
/// Add a character-based range to the currently-active diagnostic.
InflightDiagnostic CodeFixer::HighlightChars(SrcLoc sartLoc, SrcLoc endLoc) {

  assert(inflightDiag &&
         "CodeFixer can only be invoked by an InflightDiagnostic");

  return InflightDiagnostic(inflightDiag->GetDiagEngine());
}
