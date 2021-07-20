#ifndef STONE_CORE_CODEGEN_EDIAGNOSTIC_H
#define STONE_CORE_CODEGEN_DIAGNOSTIC_H

#include "stone/Core/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "CodeGenDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
