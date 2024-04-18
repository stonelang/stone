#ifndef STONE_DIAG_SYNTAXDIAGNOSTIC_H
#define STONE_DIAG_SYNTAXDIAGNOSTIC_H

#include "stone/Support/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "ASTDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
