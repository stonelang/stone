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


namespace {
// Get an official count of all of the diagnostics in the system
enum LocalDiagID : uint32_t {
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  ENUM,
#include "stone/Diag/AllDiagnosticKind.inc"
  TotalDiags
};
}

namespace {

struct StaticDiagInfoRec;

// Store the descriptions in a separate table to avoid pointers that need to
// be relocated, and also decrease the amount of data needed on 64-bit
// platforms. See "How To Write Shared Libraries" by Ulrich Drepper.
struct StaticDiagInfoDescriptionStringTable {
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  char ENUM##_desc[sizeof(DESC)];
  // clang-format off
#include "stone/Diag/DiagnosticBasicKind.inc"
#include "stone/Diag/DiagnosticParseKind.inc"

  // clang-format on
#undef DIAG
};


const StaticDiagInfoDescriptionStringTable StaticDiagInfoDescriptions = {
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  DESC,
// clang-format off
#include "stone/Diag/DiagnosticBasicKind.inc"
#include "stone/Diag/DiagnosticParseKind.inc"
  // clang-format on
#undef DIAG
};

extern const StaticDiagInfoRec StaticDiagInfo[];


} // namespace
