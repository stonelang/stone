#ifndef STONE_DIAG_/AST/DIAGNOSTIC_H
#define STONE_DIAG_/AST/DIAGNOSTIC_H

#include "stone/Diag/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "ASTDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
