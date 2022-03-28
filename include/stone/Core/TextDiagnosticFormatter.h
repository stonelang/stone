#ifndef STONE_CORE_TEXTDIAGNOSTICFORMATTER_H
#define STONE_CORE_TEXTDIAGNOSTICFORMATTER_H

#include <assert.h>

#include "stone/Core/DiagnosticFormatter.h"

namespace stone {

class TextDiagnosticFormatter final : public DiagnosticFormatter {
public:
  TextDiagnosticFormatter();
  ~TextDiagnosticFormatter();

public:
  void FormatText(const DiagnosticFormatOptions &fmtOptions) override;

public:
};
} // namespace stone
#endif
