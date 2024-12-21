#ifndef STONE_DIAG_DIAGNOSTICID_H
#define STONE_DIAG_DIAGNOSTICID_H

#include <stdint.h>
#include <tuple>

namespace stone {
namespace diags {
// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in Diagnostics.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... ArgTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID ID;
};

/// Describes how to pass a diagnostic argument of the given type.
///
/// By default, diagnostic arguments are passed by value, because they
/// tend to be small. Larger diagnostic arguments
/// need to specialize this class template to pass by reference.
template <typename T> struct PassArgument {
  typedef T type;
};

template <class... ArgTypes>
using DiagArgTuple = std::tuple<typename PassArgument<ArgTypes>::type...>;

} // namespace diags
} // namespace stone

#endif