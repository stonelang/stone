#include "stone/Diag/DiagnosticFormatter.h"
#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEngine.h"

using namespace stone;

DiagnosticFormatter::DiagnosticFormatter() {}

DiagnosticFormatter::~DiagnosticFormatter() = default;

bool DiagnosticFormatter::FormatDiagnostic(
    llvm::raw_ostream &OS, SrcMgr &SM, const DiagnosticImpl &DC,
    DiagnosticFormatOptions FormatOpts) const {
  return true;
}
