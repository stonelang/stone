#ifndef STONE_UTILS_COMPILE_DIAGNOSTIC_H
#define STONE_UTILS_COMPILE_DIAGNOSTIC_H

#include "stone/Utils/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "CompileDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
