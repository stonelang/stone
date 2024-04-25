#ifndef STONE_DIAG_DDIAGNOSTICSAST_H
#define STONE_DIAG_DDIAGNOSTICSAST_H

#include "stone/Syntax/DiagnosticsBasic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "DiagnosticsAST.def"
} // namespace diag
} // namespace stone

#endif
