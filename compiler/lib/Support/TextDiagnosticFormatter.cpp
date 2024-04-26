#include "stone/Support/TextDiagnosticFormatter.h"

using stone::TextDiagnosticFormatter;

TextDiagnosticFormatter::TextDiagnosticFormatter() {}

TextDiagnosticFormatter::~TextDiagnosticFormatter() {}

void TextDiagnosticFormatter::Format(ColorStream &out,
                                     const Diagnostic &diagnostic,
                                     DiagnosticFormatOptions fmtOpts) {}

void TextDiagnosticFormatter::Format(ColorStream &out, llvm::StringRef text,
                                     llvm::ArrayRef<DiagnosticArgument> args,
                                     DiagnosticFormatOptions fmtOpts) {}

void TextDiagnosticFormatter::FormatArgument(
    ColorStream &out, llvm::StringRef modifier,
    llvm::StringRef modifierArguments, llvm::ArrayRef<DiagnosticArgument> args,
    unsigned argIndex, DiagnosticFormatOptions fmtOpts) {}