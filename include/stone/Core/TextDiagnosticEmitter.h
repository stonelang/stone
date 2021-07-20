#ifndef STONE_CORE_TEXTDIAGNOSTICEMITTER_H
#define STONE_CORE_TEXTDIAGNOSTICEMITTER_H

#include "stone/Core/DiagnosticEmitter.h"
namespace stone {

class TextDiagnosticListener;
class TextDiagnosticEmitter final : public DiagnosticEmitter {
public:
  TextDiagnosticEmitter(TextDiagnosticListener *listener);
  ~TextDiagnosticEmitter();

public:
  void EmitLevel() override;
  void EmitMessage() override;
  void EmitLoc() override;
};
} // namespace stone
#endif
