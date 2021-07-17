#include "stone/Basic/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::DiagnosticEngine;

void Diagnostic::Format(llvm::SmallVectorImpl<char> &outStr) const {}

/// Format the given format-string into the output buffer using the
/// arguments stored in this diagnostic.
void Diagnostic::Format(const char *diagStr, const char *diagEnd,
                        llvm::SmallVectorImpl<char> &outStr) const {}