#ifndef STONE_BASIC_TEXTDIAGNOSTICEMITTER_H
#define STONE_BASIC_TEXTDIAGNOSTICEMITTER_H

#include <assert.h>

namespace stone {

class TextDiagnosticFormatter  : public DiagnosticFormatter {
public:
  TextDiagnosticFormatter();
  ~TextDiagnosticFormatter();
public:
};
} // namespace stone
#endif
