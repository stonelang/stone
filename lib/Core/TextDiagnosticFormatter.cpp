#include "stone/Core/TextDiagnosticFormatter.h"

using stone::TextDiagnosticFormatter;

TextDiagnosticFormatter::TextDiagnosticFormatter() {}

TextDiagnosticFormatter::~TextDiagnosticFormatter() {}

void TextDiagnosticFormatter::Format(llvm::raw_ostream &out,
                                     DiagnosticDetail &detail,
                                     DiagnosticFormatOptions &fmtOpts) {}

void TextDiagnosticFormatter::Format(llvm::raw_ostream &out,
                                     llvm::StringRef text,
                                     llvm::ArrayRef<diag::Argument> args,
                                     DiagnosticFormatOptions &fmtOpts) {}
