#ifndef STONE_DIAG_DIAGNOSTICOPTIONS_H
#define STONE_DIAG_DIAGNOSTICOPTIONS_H

#include "llvm/ADT/Hashing.h"

namespace stone {

/// Describes the current behavior to take with a diagnostic.
/// Ordered from most severe to least.
enum class DiagnosticBehavior : uint8_t {
  Unspecified = 0,
  Fatal,
  Error,
  Warning,
  Remark,
  Note,
  Ignore,
};

/// Options for controlling diagnostics.
class DiagnosticOptions final {
public:
  // TODO: remove this note: (d1Start = 1, d1End = d1Start + max)
  // (d2Start = d1End + 1  , d2End = d1End + max)
  unsigned int maxMessagesPerDiagnostic = 100;

  enum FormattingStyle { LLVM, Stone };
  // If set to true, use the more descriptive experimental formatting style for
  // diagnostics.
  FormattingStyle formattingStyle = FormattingStyle::LLVM;

  DiagnosticBehavior diagnosticLevel = DiagnosticBehavior::Unspecified;

  /// TODO: This is a copy of what is in DiagnosticEngine -- may live here.
  // Treat fatal errors like errors.
  bool fatalsAsError = false;

  // Suppress all diagnostics.
  bool suppressAllDiagnostics = false;

  // Elide common types of templates.
  bool elideType = true;

  // Print a tree when comparing templates.
  bool printTemplateTree = false;

  /// Indicates whether textual diagnostics should use color.
  bool useColor = false;
  // Which overload candidates to show.
  // OverloadsShown ShowOverloads = Ovl_All;

  // Cap of # errors emitted, 0 -> no limit.
  unsigned errorLimit = 0;

  bool ownsListener = false;

  /// Write directly to the console
  bool enableDebugging = false;
};

} // namespace stone

#endif