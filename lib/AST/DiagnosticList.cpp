#include "stone/AST/DiagnosticsBasic.h"

using namespace stone;

enum class stone::DiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/AST/Diagnostics.def"
};
static_assert(static_cast<uint32_t>(stone::DiagID::invalid_diagnostic) == 0,
              "0 is not the invalid diagnostic ID");

enum class stone::FixItID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature)
#define FIXIT(ID, Text, Signature) ID,
#include "stone/AST/Diagnostics.def"
};

// Define all of the diagnostic objects and initialize them with their
// diagnostic IDs.
namespace stone {
namespace diag {
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  detail::DiagWithArguments<void Signature>::type ID = {DiagID::ID};

#define FIXIT(ID, Text, Signature)                                             \
  detail::StructuredFixItWithArguments<void Signature>::type ID = {FixItID::ID};

#include "stone/AST/Diagnostics.def"
} // end namespace diag
} // end namespace stone
