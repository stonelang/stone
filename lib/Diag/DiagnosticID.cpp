
#include "stone/Diag/DiagnosticBasicKind.h"
using namespace stone;

enum class diags::DiagID : uint32_t {
#define DIAG(KIND, ID, Options, Message, Signature) ID,
#include "stone/Diag/DiagnosticEngine.def"
};
static_assert(static_cast<uint32_t>(diags::DiagID::error_invalid_diagnostic) == 0,
              "0 is not the invalid diagnostic ID");

// Define all of the diagnostic objects and initialize them with their
// diagnostic IDs.
namespace stone {
namespace diags {
#define DIAG(KIND, ID, Options, Message, Signature)                            \
  DiagWithArguments<void Signature>::type ID = {DiagID::ID};

#include "stone/Diag/DiagnosticEngine.def"
} // namespace diags
} // end namespace stone
