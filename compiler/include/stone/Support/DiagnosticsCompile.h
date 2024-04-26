#ifndef STONE_DIAG_DIAGNOSTICCOMPILE_H
#define STONE_DIAG_DIAGNOSTICCOMPILE_H

#include "stone/Support/DiagnosticsCore.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "DiagnosticsCompile.def"
} // namespace diag
} // namespace stone

#endif
