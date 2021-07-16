#ifndef STONE_SYNTAX_IDENTIFIERDIAGNOSABLE_H
#define STONE_SYNTAX_IDENTIFIERDIAGNOSABLE_H

#include "stone/Basic/Diagnosable.h"
#include "stone/Syntax/Identifier.h"

#include <assert.h>

namespace stone {

class DiagnosticEngine;

class IdentifierDiagnosable : public Diagnosable {
  const syn::Identifier &identifier;

public:
  IdentifierDiagnosable(const syn::Identifier &identifier)
      : identifier(identifier) {}

public:
  void Diagnose(DiagnosticEngine &de) override;
};

} // namespace stone

#endif
