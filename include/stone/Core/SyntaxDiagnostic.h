#ifndef STONE_CORE_SYNTAXDIAGNOSTIC_H
#define STONE_CORE_SYNTAXDIAGNOSTIC_H

#include "stone/Core/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "SyntaxDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
