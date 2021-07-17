#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Basic/Diagnostic.h"

using stone::Diagnostic;
using stone::TextBufferingDiagnosticListener;
using stone::TextDiagnosticListener;

TextBufferingDiagnosticListener::TextBufferingDiagnosticListener() {}

void TextBufferingDiagnosticListener::Listen(diag::Level,
                                             const Diagnostic &diagnostic) {}

TextDiagnosticListener::TextDiagnosticListener() {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::Listen(diag::Level, const Diagnostic &diagnostic) {
}
