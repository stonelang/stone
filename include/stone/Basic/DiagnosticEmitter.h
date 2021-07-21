#ifndef STONE_BASIC_DIAGNOSTICEMITTER_H
#define STONE_BASIC_DIAGNOSTICEMITTER_H

#include <assert.h>

namespace stone {
class DiagnosticListener;
class DiagnosticEmitter {

  DiagnosticListener *listener;

public:
  DiagnosticEmitter(DiagnosticListener *listener);
  virtual ~DiagnosticEmitter();

public:
  virtual void EmitLevel();
  virtual void EmitMessage();
  virtual void EmitLoc();

public:
  DiagnosticListener *GetListener() {
    assert(
        listener &&
        "A 'DiagnosticEmitter' must be associated with a 'DiagnosticListener'");
    return listener;
  }
};
} // namespace stone
#endif
