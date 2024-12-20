#ifndef STONE_DIAG_DIAGNOSTICID_H
#define STONE_DIAG_DIAGNOSTICID_H

#include <stdint.h>

namespace stone {
namespace diags {
// enum DiagID : unsigned;

// // Enumeration describing all of possible diagnostics.
// ///
// /// Each of the diagnostics described in Diagnostics.def has an entry in
// /// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;

/// Describes a diagnostic with no arguments
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  typedef DiagID ID;
};

} // namespace diags
} // namespace stone

#endif