#ifndef STONE_DIAG_DRIVERDIAGNOSTIC_H
#define STONE_DIAG_DRIVERDIAGNOSTIC_H

#include "stone/Support/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "DriverDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
