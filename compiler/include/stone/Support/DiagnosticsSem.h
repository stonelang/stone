#ifndef STONE_DIAG_SEMDIAGNOSTIC_H
#define STONE_DIAG_SEMDIAGNOSTIC_H

#include "stone/Support/DiagnosticsCore.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "DiagnosticsSem.def"
} // namespace diag
} // namespace stone

#endif
