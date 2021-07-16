#ifndef STONE_SYNTAX_SYNTAXDIAGARGUMENT_H
#define STONE_SYNTAX_SYNTAXDIAGARGUMENT_H

#include "stone/Basic/DiagnosticArgument.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"

#include <assert.h>

namespace stone {

class DiagnosticEngine;

struct DeclDiagnosticArgument final : public ComplexDiagnosticArgument {
public:
  DeclDiagnosticArgument(const syn::Decl *val)
      : ComplexDiagnosticArgument(val) {}

public:
  void Diagnose(DiagnosticEngine &de) override;
};

struct DeclContextDiagnosticArgument final : public ComplexDiagnosticArgument {
public:
  DeclContextDiagnosticArgument(const syn::DeclContext *val)
      : ComplexDiagnosticArgument(val) {}

public:
  void Diagnose(DiagnosticEngine &de) override;
};

struct IdentifierDiagnosticArgument final : public ComplexDiagnosticArgument {
public:
  IdentifierDiagnosticArgument(const syn::Identifier *val)
      : ComplexDiagnosticArgument(val) {}

public:
  void Diagnose(DiagnosticEngine &de) override;
};

} // namespace stone

#endif
