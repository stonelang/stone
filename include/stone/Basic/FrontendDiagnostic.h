#ifndef STONE_BASIC_FRONTENDDIAGNOSTIC_H
#define STONE_BASIC_FRONTENDDIAGNOSTIC_H

#include "stone/Basic/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "FrontendDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
