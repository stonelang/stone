#ifndef STONE_CORE_DIAGNOSTICLISTENER_H
#define STONE_CORE_DIAGNOSTICLISTENER_H

#include "stone/Core/Color.h"
#include "stone/Core/DiagnosticFormatter.h"
#include "stone/Core/DiagnosticOptions.h"

namespace stone {

class DiagnosticEvent;

class DiagnosticListener {

protected:
  ColorfulStream colorfulStream;
  DiagnosticFormatter *formatter = nullptr;

  unsigned numWarnings = 0; ///< Number of warnings reported
  unsigned numErrors = 0;   ///< Number of errors reported

public:
  DiagnosticListener();
  virtual ~DiagnosticListener();

  unsigned GetNumErrors() const { return numErrors; }
  unsigned GetNumWarnings() const { return numWarnings; }

  /// Clear all warnings and errors -- relplace with Flush()
  virtual void Clear() { numWarnings = numErrors = 0; }

  /// Callback to inform the diagnostic client that processing of all
  /// source files has ended.
  virtual void Finish();

  virtual void Flush();

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
  virtual void OnDiagnostic(const DiagnosticEvent &diagnostic);

  void SetFormatter(DiagnosticFormatter *diagFormatter) {
    assert(diagFormatter);
    formatter = diagFormatter;
  }
  DiagnosticFormatter *GetFormatter() { return formatter; }
};

class FakeDiagnosticListener final : public DiagnosticListener {
public:
  void OnDiagnostic(const DiagnosticEvent &diagEvent) override {}
};
} // namespace stone
#endif
