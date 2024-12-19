#ifndef STONE_DIAG_TEXT_DIAGNOSTIC_EMITTER_H
#define STONE_DIAG_TEXT_DIAGNOSTIC_EMITTER_H

#include "stone/Diagnostics/DiagnosticEmitter.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {

namespace diags {

class TextDiagnosticEmitter final : public DiagnosticEmitter {
public:
  TextDiagnosticEmitter(llvm::raw_ostream &OS, const LangOptions &LangOpts,
                        DiagnosticOptions &DiagOpts);
  ~TextDiagnosticEmitter() override;

protected:
  void EmitDiagnosticMessage(FullSrcLoc Loc, PresumedLoc PLoc,
                             DiagnosticLevel Level, llvm::StringRef Message,
                             ArrayRef<CharSrcRange> Ranges,
                             DiagnosticInfoOrStoredDiagnotic Info) override;

  void EmitDiagnosticLoc(FullSrcLoc Loc, PresumedLoc PLoc,
                         DiagnosticLevel Level,
                         ArrayRef<CharSrcRange> Ranges) override;

  void EmitCodeContext(FullSrcLoc Loc, DiagnosticLevel Level,
                       llvm::SmallVectorImpl<CharSrcRange> &Ranges,
                       llvm::ArrayRef<FixIt> FixIts) override;
};

} // namespace diags

} // namespace stone

#endif