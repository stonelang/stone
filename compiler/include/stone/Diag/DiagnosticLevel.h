#ifndef STONE_DIAG_DIAGNOSTIC_LEVEL_H

namespace stone {
namespace diags {

/// The level of the diagnostic, after it has been through mapping.
enum class DiagnosticLevel {
  Ignored = 0,
  Note,
  Remark,
  Warning,
  Error,
  Fatal,
};

} // namespace diags
} // namespace stone

#endif
