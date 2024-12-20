#ifndef STONE_DIAG_DIAGNOSTICSCOMMON_H
#define STONE_DIAG_DIAGNOSTICSCOMMON_H

#include "stone/Diag/DiagnosticID.h"
#include "stone/Basic/LLVM.h"
// #include "stone/Config.h"

namespace stone {
namespace diags {

struct Diag;
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  extern Diag::ID ENUM;
#include "stone/Diag/DiagnosticBasicKind.inc"

} // namespace diags

} // end namespace stone

#endif
