#ifndef STONE_BASIC_TEXTDIAGNOSTICEMITTER_H
#define STONE_BASIC_TEXTDIAGNOSTICEMITTER_H

#include "stone/Basic/DiagnosticEmitter.h"
namespace stone {

class TextDiagnosticEmitter final : public DiagnosticEmitter {
public:
public:
  TextDiagnosticEmitter();
  ~TextDiagnosticEmitter();
};
} // namespace stone
#endif
