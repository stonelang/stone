#include "stone/Basic/DiagnosticEngine.h"

using stone::CodeFix;
using stone::CodeFixer;
using stone::Diagnostic;
using stone::DiagnosticEngine;
using stone::LiveDiagnostic;

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

/// Create a code modification hint that inserts the given
/// code string at a specific location.
LiveDiagnostic CodeFixer::Insert(SrcLoc insertionLoc, StringRef code,
                                 bool beforePreviousInsertions) {
  CodeFix fix;
  fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
  fix.codeToInsert = std::string(code);
  fix.beforePreviousInsertions = beforePreviousInsertions;

  return LiveDiagnostic(de);
}

/// Create a code modification hint that inserts the given
/// code from \p FromRange at a specific location.
LiveDiagnostic CodeFixer::InsertFromRange(SrcLoc insertionLoc, CharSrcRange fromRange,
                                   bool beforePreviousInsertions) {
  CodeFix fix;
  fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
  fix.insertFromRange = fromRange;
  fix.beforePreviousInsertions = beforePreviousInsertions;
  
  return LiveDiagnostic(de);;
}
/// Create a code modification hint that removes the given
/// source range.
LiveDiagnostic CodeFixer::Remove(CharSrcRange removeRange) {
  CodeFix fix;
  fix.removeRange = removeRange;
  return LiveDiagnostic(de);;
}
LiveDiagnostic CodeFixer::Remove(SrcRange removeRange) {
  return Remove(CharSrcRange::getTokenRange(removeRange));
}

/// Create a code modification hint that replaces the given
/// source range with the given code string.
LiveDiagnostic CodeFixer::Replace(CharSrcRange removeRange, llvm::StringRef code) {
  CodeFix fix;
  fix.removeRange = removeRange;
  fix.codeToInsert = std::string(code);
  return LiveDiagnostic(de);;
}

LiveDiagnostic CodeFixer::Replace(SrcRange removeRange, llvm::StringRef code) {
  return Replace(CharSrcRange::getTokenRange(removeRange), code);
}