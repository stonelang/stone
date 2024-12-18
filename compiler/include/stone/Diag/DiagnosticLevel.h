#ifndef STONE_DIAG_DIAGNOSTIC_LEVEL_H
#define STONE_DIAG_DIAGNOSTIC_LEVEL_H

namespace stone {
namespace diags {

/// The level of the diagnostic, after it has been through mapping.
enum class DiagnosticLevel {
  None = 0,
  /// Lowest
  Ignored,
  Note,
  Remark,
  Warning,
  Error,
  /// Highest
  Fatal,
};

} // namespace diags
} // namespace stone

#endif
