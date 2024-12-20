#include "stone/Diag/DiagnosticEmitter.h"

using namespace stone;

diags::DiagnosticEmitter::DiagnosticEmitter(DiagnosticOutputStream &OS,
                                            const LangOptions &LangOpts,
                                            const DiagnosticOptions &DiagOpts)
    : OS(OS), LangOpts(LangOpts), DiagOpts(DiagOpts), LastLevel() {}

diags::DiagnosticEmitter::~DiagnosticEmitter() = default;