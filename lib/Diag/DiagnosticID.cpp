#include "stone/Diag/DiagnosticID.h"

using namespace stone;

enum class diags::DiagID : uint32_t {
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  ENUM,
#include "stone/Diag/AllDiagnosticKind.inc"
};

namespace stone {
namespace diags {
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  Diag::ID ENUM = {DiagID::ENUM};
#include "stone/Diag/AllDiagnosticKind.inc"
} // namespace diags
} // end namespace stone
