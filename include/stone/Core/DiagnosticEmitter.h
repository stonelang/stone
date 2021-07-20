#ifndef STONE_CORE_DIAGNOSTICEMITTER_H
#define STONE_CORE_DIAGNOSTICEMITTER_H

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
  DiagnosticListener *GetListener() { return listener; }
};
} // namespace stone
#endif
