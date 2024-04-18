#ifndef STONE_DIAG_DIAGNOSTICOPTIONS_H
#define STONE_DIAG_DIAGNOSTICOPTIONS_H

#include "llvm/ADT/Hashing.h"

namespace stone {
namespace diag {
enum class Level : uint8_t {
  None = 0,
  // Highest
  Fatal,
  Error,
  Warn,
  Remark,
  Note,
  // Lowest
  Ignore,
};
}
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

  diag::Level diagnosticLevel = diag::Level::None;

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
