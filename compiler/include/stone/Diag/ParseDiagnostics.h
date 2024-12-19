#ifndef STONE_DIAG_DIAGNOSTICSPARSE_H
#define STONE_DIAG_DIAGNOSTICSPARSE_H

#include "stone/AST/DiagnosticsBasic.h"

namespace stone {
  /// TODO: Use ParseDiagnostics.inc
namespace diags {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "ParseDiagnostics.inc"
} // namespace diag
} // namespace stone

#endif
