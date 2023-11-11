#ifndef STONE_DIAG_DIAGNOSTICLISTENER_H
#define STONE_DIAG_DIAGNOSTICLISTENER_H

#include "stone/Diag/DiagnosticEmitter.h"
#include "stone/Diag/DiagnosticOptions.h"

namespace stone {

class DiagnosticMessage;

class DiagnosticListener {

protected:
  DiagnosticEmitter &emitter;

  unsigned numWarnings = 0; ///< Number of warnings reported
  unsigned numErrors = 0;   ///< Number of errors reported

  bool forceColors = false;
  bool didErrorOccur = false;

public:
  // TODO: May just waht to pass by value
  DiagnosticListener(DiagnosticEmitter &emitter);
  virtual ~DiagnosticListener();

  unsigned GetNumErrors() const { return numErrors; }
  unsigned GetNumWarnings() const { return numWarnings; }

  /// Clear all warnings and errors -- relplace with Flush()
  virtual void Clear() { numWarnings = numErrors = 0; }

  /// Callback to inform the diagnostic client that processing of all
  /// source files has ended.
  virtual bool Finish();

  /// Indicates whether the diagnostics handled by this
  /// DiagnosticListener should be included in the number of diagnostics
  /// reported by DiagnosticEngine.
  ///
  /// The default implementation returns true.
  virtual bool IncludeInDiagnosticCounts() const;

  // TODO: May consider pasing source manager -- or pass Context in Diagnostics
  /// Handle this diagnostic, reporting it to the user or
  /// capturing it to a log as needed.
  ///
  /// The default implementation just keeps track of the total number of
  /// warnings and errors.
  virtual void OnDiagnostic(const DiagnosticMessage &diagEvent) = 0; 

  // void SetFormatter(DiagnosticFormatter *diagFormatter) {
  //   assert(diagFormatter);
  //   formatter = diagFormatter;
  // }

  DiagnosticEmitter &GetEmitter() { return emitter; }

  void SetForceColors(bool useColors = false) { forceColors = useColors; }
};

// class FakeDiagnosticListener final : public DiagnosticListener {

// public:

// public:
//   void OnDiagnostic(const DiagnosticMessage &diagEvent) override {}
// };

} // namespace stone
#endif
