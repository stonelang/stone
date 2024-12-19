#include "stone/Diag/TextDiagnosticEmitter.h"

using namespace stone;

diags::DiagnosticEmitter::DiagnosticEmitter(const LangOptions &LangOpts,
                                            DiagnosticOptions &DiagOpts)
    : LangOpts(LangOpts), DiagOpts(DiagOpts), LastLevel() {}

diags::DiagnosticEmitter::~DiagnosticEmitter() = default;