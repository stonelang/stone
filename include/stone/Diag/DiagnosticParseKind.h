#ifndef STONE_DIAG_DIAGNOSTICSPARSE_H
#define STONE_DIAG_DIAGNOSTICSPARSE_H

#include "stone/Diag/DiagnosticBasicKind.h"

namespace stone {
namespace diags {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Message, Signature)                            \
  extern DiagWithArguments<void Signature>::type ID;
#include "DiagnosticParseKind.def"

} // namespace diags
} // namespace stone

#endif
