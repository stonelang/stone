#ifndef STONE_DIAG_DIAGNOSTICSPARSE_H
#define STONE_DIAG_DIAGNOSTICSPARSE_H

#include "stone/Diag/DiagnosticBasicKind.h"

namespace stone {
namespace diags{
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  extern Diag::ID ENUM;
#include "stone/Diag/DiagnosticParseKind.inc"
} // namespace diags
} // namespace stone

#endif
