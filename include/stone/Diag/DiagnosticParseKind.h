#ifndef STONE_DIAG_DIAGNOSTICSPARSE_H
#define STONE_DIAG_DIAGNOSTICSPARSE_H

#include "stone/Diag/DiagnosticBasicKind.h"

namespace stone {
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  extern Diag::ID ENUM;
#include "stone/AST/DiagnosticParseKind.inc"
} // namespace diags
} // namespace stone

#endif
