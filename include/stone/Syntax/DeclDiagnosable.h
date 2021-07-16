#ifndef STONE_SYNTAX_DECLDIAGNOSABLE_H
#define STONE_SYNTAX_DECLDIAGNOSABLE_H

#include "stone/Basic/Diagnosable.h"
#include "stone/Syntax/Decl.h"
#include <assert.h>

namespace stone {

class DiagnosticEngine;

class DeclDiagnosable : public Diagnosable {
  const syn::Decl &declaration;

public:
  DeclDiagnosable(const syn::Decl &declaration) : declaration(declaration) {}

public:
  void Diagnose(DiagnosticEngine &de) override;
};

} // namespace stone

#endif
