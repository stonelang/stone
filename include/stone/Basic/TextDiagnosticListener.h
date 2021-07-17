#ifndef STONE_BASIC_TEXTDIAGNOSTICLISTENER_H
#define STONE_BASIC_TEXTDIAGNOSTICLISTENER_H

#include "stone/Basic/DiagnosticListener.h"

namespace stone {

class TextBufferingDiagnosticListener final : public DiagnosticListener {
public:
};

class TextDiagnosticListener final : public DiagnosticListener {
public:
  TextDiagnosticListener();
  ~TextDiagnosticListener();
};
} // namespace stone

#endif
