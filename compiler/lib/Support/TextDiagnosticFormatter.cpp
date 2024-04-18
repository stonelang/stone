#include "stone/Support/TextDiagnosticFormatter.h"

using stone::TextDiagnosticFormatter;

TextDiagnosticFormatter::TextDiagnosticFormatter() {}

TextDiagnosticFormatter::~TextDiagnosticFormatter() {}

void TextDiagnosticFormatter::Format(ColorStream &out,
                                     const Diagnostic &diagnostic,
                                     DiagnosticFormatOptions fmtOpts) {}

void TextDiagnosticFormatter::Format(ColorStream &out, llvm::StringRef text,
                                     llvm::ArrayRef<diag::Argument> args,
                                     DiagnosticFormatOptions fmtOpts) {}

void TextDiagnosticFormatter::FormatArgument(
    ColorStream &out, llvm::StringRef modifier,
    llvm::StringRef modifierArguments, llvm::ArrayRef<diag::Argument> args,
    unsigned argIndex, DiagnosticFormatOptions fmtOpts) {}