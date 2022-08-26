#include "stone/Diag/CoreDiagnostic.h"

using namespace stone;

enum class stone::DiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/Diag/DiagnosticEngine.def"
};
static_assert(static_cast<uint32_t>(stone::DiagID::invalid_diagnostic) == 0,
              "0 is not the invalid diagnostic ID");

enum class stone::FixID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature)
#define FIX(ID, Text, Signature) ID,
#include "stone/Diag/DiagnosticEngine.def"
};

// Define all of the diagnostic objects and initialize them with their
// diagnostic IDs.
namespace stone {
namespace diag {
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  detail::DiagWithArguments<void Signature>::type ID = {DiagID::ID};
#include "stone/Diag/DiagnosticEngine.def"


//TODO: 
// #define FIX(ID, Text, Signature)                                               \
//   detail::FixWithArguments<void Signature>::type ID = {FixID::ID};
// #include "stone/Diag/DiagnosticEngine.def"

} // end namespace diag
} // end namespace stone
