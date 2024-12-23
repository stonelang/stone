
#include "stone/Diag/DiagnosticBasicKind.h"
using namespace stone;

enum class stone::DiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/Diag/DiagnosticEngine.def"
};
static_assert(static_cast<uint32_t>(stone::DiagID::error_invalid_diagnostic) ==
                  0,
              "0 is not the invalid diagnostic ID");

// Define all of the diagnostic objects and initialize them with their
// diagnostic IDs.
namespace stone {
namespace diag {
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  detail::DiagWithArguments<void Signature>::type ID = {DiagID::ID};
#include "stone/Diag/DiagnosticEngine.def"
} // end namespace diag
} // end namespace stone
