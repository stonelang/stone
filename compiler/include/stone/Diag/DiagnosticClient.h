#ifndef STONE_DIAG_DIAGNOSTIC_CLIENT_H
#define STONE_DIAG_DIAGNOSTIC_CLIENT_H

#include "llvm/ADT/SmallVector.h"

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

class FixIt final {
public:
};

class DiagnosticInfo final {
public:
  /// Format this diagnostic into a string, substituting the
  /// formal arguments into the %0 slots.
  ///
  /// The result is appended onto the \p OutStr array.
  void FormatDiagnostic(llvm::SmallVectorImpl<char> &OutStr) const;

  /// Format the given format-string into the output buffer using the
  /// arguments stored in this diagnostic.
  void FormatDiagnostic(const char *DiagStr, const char *DiagEnd,
                        llvm::SmallVectorImpl<char> &OutStr) const;
};

class DiagnosticClient {

protected:
  unsigned NumWarnings = 0; ///< Number of warnings reported
  unsigned NumErrors = 0;   ///< Number of errors reported

public:
  DiagnosticClient();
  virtual ~DiagnosticClient();

public:
  virtual void Clear() { NumWarnings = NumErrors = 0; }

  /// Callback to inform the diagnostic client that processing of all
  /// source files has ended.
  virtual void FinishProcessing() {}

  /// Indicates whether the diagnostics handled by this
  /// DiagnosticConsumer should be included in the number of diagnostics
  /// reported by DiagnosticsEngine.
  ///
  /// The default implementation returns true.
  virtual bool UseInDiagnosticCounts() const;

  /// Handle this diagnostic, reporting it to the user or
  /// capturing it to a log as needed.
  ///
  /// The default implementation just keeps track of the total number of
  /// warnings and errors.
  virtual void HandleDiagnostic(DiagnosticLevel DiagLevel,
                                const DiagnosticInfo &Info);
};

class NullDiagnosticClient final : public DiagnosticClient {
public:
  void HandleDiagnostic(DiagnosticLevel DiagLevel,
                        const DiagnosticInfo &Info) override {
    // Just ignore it.
  }
};

/// Diagnostic consumer that forwards diagnostics along to an
/// existing, already-initialized diagnostic consumer.
///
class ForwardingDiagnosticClient final : public DiagnosticClient {

public:
};

} // namespace diags

} // namespace stone

#endif