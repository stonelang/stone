#ifndef STONE_UTILS_SYNDIAGNOSTIC_H
#define STONE_UTILS_SYNDIAGNOSTIC_H

#include "stone/Utils/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "SynDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
