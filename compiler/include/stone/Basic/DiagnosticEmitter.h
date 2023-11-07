#ifndef STONE_DIAG_DIAGNOSTICEMITTER_H
#define STONE_DIAG_DIAGNOSTICEMITTER_H

#include "stone/Basic/Color.h"
#include "stone/Basic/DiagnosticFormatter.h"

#include <assert.h>

namespace stone {

class DiagnosticMessage;
class DiagnosticFormatter;

class DiagnosticEmitter {

  DiagnosticFormatter &formatter;

public:
  DiagnosticEmitter() = delete;

  DiagnosticEmitter(DiagnosticFormatter &formatter);
  virtual ~DiagnosticEmitter();

public:
  virtual void EmitLevel();
  virtual void EmitDiagnostic(const DiagnosticMessage &de);
  virtual void EmitLoc();

public:
  DiagnosticFormatter &GetFormatter() { return formatter; }
};

} // namespace stone
#endif