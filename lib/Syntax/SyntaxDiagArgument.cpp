#include "stone/Syntax/SyntaxDiagArgument.h"
#include "stone/Basic/DiagnosticEngine.h"

using namespace stone;

void DeclDiagnosticArgument::Diagnose(DiagnosticEngine &de) {}

void DeclContextDiagnosticArgument::Diagnose(DiagnosticEngine &de) {}

void IdentifierDiagnosticArgument::Diagnose(DiagnosticEngine &de) {
  de.Issue(SrcLoc(), 1) << "test";
}
