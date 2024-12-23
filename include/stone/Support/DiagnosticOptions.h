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

struct DiagnosticFormatOptions {
  const std::string OpeningQuotationMark;
  const std::string ClosingQuotationMark;
  const std::string AKAFormatString;
  const std::string OpaqueResultFormatString;

  DiagnosticFormatOptions(std::string OpeningQuotationMark,
                          std::string ClosingQuotationMark,
                          std::string AKAFormatString,
                          std::string OpaqueResultFormatString)
      : OpeningQuotationMark(OpeningQuotationMark),
        ClosingQuotationMark(ClosingQuotationMark),
        AKAFormatString(AKAFormatString),
        OpaqueResultFormatString(OpaqueResultFormatString) {}

  DiagnosticFormatOptions()
      : OpeningQuotationMark("'"), ClosingQuotationMark("'"),
        AKAFormatString("'%s' (aka '%s')"),
        OpaqueResultFormatString("'%s' (%s of '%s')") {}

  /// When formatting fix-it arguments, don't include quotes or other
  /// additions which would result in invalid code.
  static DiagnosticFormatOptions formatForFixIts() {
    return DiagnosticFormatOptions("", "", "%s", "%s");
  }
};

/// Options for controlling diagnostics.
class DiagnosticOptions final {
public:
  // TODO: remove this note: (d1Start = 1, d1End = d1Start + max)
  // (d2Start = d1End + 1  , d2End = d1End + max)
  unsigned int maxMessagesPerDiagnostic = 100;

  unsigned int LinesAfterEachDiagnosticMessage = 2;

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
  bool showColors = false;
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
