#include "stone/Compile/CompilerInvocation.h"

using namespace stone;

CompilerDiagnosticFormatter::CompilerDiagnosticFormatter() {}

void CompilerDiagnosticFormatter::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText,
    ArrayRef<DiagnosticArgument> FormatArgs,
    DiagnosticFormatOptions FormatOpts) {}