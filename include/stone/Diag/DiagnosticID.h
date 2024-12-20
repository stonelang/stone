#ifndef STONE_DIAG_DIAGNOSTICID_H
#define STONE_DIAG_DIAGNOSTICID_H

#include <stdint.h>

namespace stone {
namespace diags {
enum class DiagnosticLevel;

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

/// Enum values that allow the client to map NOTEs, WARNINGs, and EXTENSIONs
/// to either Ignore (nothing), Remark (emit a remark), Warning
/// (emit a warning) or Error (emit as an error).  It allows clients to
/// map ERRORs to Error or Fatal (stop emitting diagnostics after this one).
enum class Severity {
  // NOTE: 0 means "uncomputed".
  Ignored = 1, ///< Do not present this diagnostic, ignore it.
  Remark = 2,  ///< Present this diagnostic as a remark.
  Warning = 3, ///< Present this diagnostic as a warning.
  Error = 4,   ///< Present this diagnostic as an error.
  Fatal = 5    ///< Present this diagnostic as a fatal error.
};

/// Flavors of diagnostics we can emit. Used to filter for a particular
/// kind of diagnostic (for instance, for -W/-R flags).
enum class Flavor {
  WarningOrError, ///< A diagnostic that indicates a problem or potential
                  ///< problem. Can be made fatal by -Werror.
  Remark          ///< A diagnostic that indicates normal progress through
                  ///< compilation.
};

class DiagnosticMapping {};

class DiagIDContext final {

public:
  DiagIDContext();

  ~DiagIDContext();

public:
  DiagID CreateCustomFromFormatString(DiagnosticLevel DiagLevel,
                                      llvm::StringRef FormatString);

  llvm::StringRef GetDescription(DiagID ID) const;

public:
  /// Return true if the unmapped diagnostic levelof the specified
  /// diagnostic ID is a Warning or Extension.
  ///
  /// This only works on builtin diagnostics, not custom ones, and is not
  /// legal to call on NOTEs.
  static bool IsBuiltinWarningOrExtension(DiagID ID);

  /// Return true if the specified diagnostic is mapped to errors by
  /// default.
  static bool IsDefaultMappingAsError(DiagID ID);

  /// Get the default mapping for this diagnostic.
  static DiagnosticMapping GetDefaultMapping(DiagID ID);

  /// Determine whether the given built-in diagnostic ID is a Note.
  static bool IsBuiltinNote(DiagID ID);
};

} // namespace diags
} // namespace stone

#endif