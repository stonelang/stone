#include "stone/Diag/TextDiagnosticFormatter.h"

using stone::TextDiagnosticFormatter;

TextDiagnosticFormatter::TextDiagnosticFormatter() {}

TextDiagnosticFormatter::~TextDiagnosticFormatter() {}

void TextDiagnosticFormatter::Format(ColorStream &out, DiagnosticDetail &detail,
                                     DiagnosticFormatOptions fmtOpts) {}

void TextDiagnosticFormatter::Format(ColorStream &out, llvm::StringRef text,
                                     llvm::ArrayRef<diag::Argument> args,
                                     DiagnosticFormatOptions fmtOpts) {}