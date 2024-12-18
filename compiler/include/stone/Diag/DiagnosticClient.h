#ifndef STONE_DIAG_DIAGNOSTIC_CLIENT_H
#define STONE_DIAG_DIAGNOSTIC_CLIENT_H

#include "stone/Diag/DiagnosticLevel.h"
#include "stone/Diag/DiagnosticInfo.h"

namespace stone {
namespace diags {

class DiagnosticClient {
public:
  DiagnosticClient();

public:
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

class BlankDiagnosticClient final : public DiagnosticClient {
public:
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