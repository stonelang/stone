#include "stone/Diag/TextDiagnosticEmitter.h"

using namespace stone;

diags::TextDiagnosticEmitter::TextDiagnosticEmitter(llvm::raw_ostream &OS,
                                                    const LangOptions &LangOpts,
                                                    DiagnosticOptions &DiagOpts)
    : DiagnosticEmitter(LangOpts, DiagOpts) {}

diags::TextDiagnosticEmitter::~TextDiagnosticEmitter() {}