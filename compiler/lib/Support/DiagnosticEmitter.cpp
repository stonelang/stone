#include "stone/Support/DiagnosticEmitter.h"
#include "stone/Support/Diagnostic.h"
#include "stone/Support/DiagnosticOptions.h"

using namespace stone;

DiagnosticEmitter::DiagnosticEmitter() {}
DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitDiagnostic(const DiagnosticMessage &diagnostic) {}
void DiagnosticEmitter::EmitLoc() {}

void DiagnosticEmitter::Format(ColorStream &out, const Diagnostic &diagnostic,
                               DiagnosticFormatOptions fmtOpts) {}

void DiagnosticEmitter::Format(ColorStream &out, llvm::StringRef text,
                               llvm::ArrayRef<DiagnosticArgument> args,
                               DiagnosticFormatOptions fmtOpts) {}

void DiagnosticEmitter::FormatArgument(ColorStream &out,
                                       llvm::StringRef modifier,
                                       llvm::StringRef modifierArguments,
                                       ArrayRef<DiagnosticArgument> args,
                                       unsigned argIndex,
                                       DiagnosticFormatOptions fmtOpts) {}
