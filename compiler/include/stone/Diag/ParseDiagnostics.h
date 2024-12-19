#ifndef STONE_DIAG_DIAGNOSTICSPARSE_H
#define STONE_DIAG_DIAGNOSTICSPARSE_H

#include "stone/Diag/BasicDiagnostics.h"

namespace stone {
/// TODO: Use ParseDiagnostics.inc
namespace diags {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, SFINAE, NOWERROR,      \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "ParseDiagnostics.inc"
} // namespace diags
} // namespace stone

#endif
