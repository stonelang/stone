#include "stone/Basic/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::DiagnosticEngine;

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