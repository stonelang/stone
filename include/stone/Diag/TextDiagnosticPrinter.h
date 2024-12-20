#ifndef STONE_DIAG_DIAGNOSTIC_PRINTER_H
#define STONE_DIAG_DIAGNOSTIC_PRINTER_H

#include "stone/Basic/LLVM.h"
#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEmitter.h"

#include <memory>

namespace stone {
namespace diags {

class TextDiagnosticEmitter final : public DiagnosticEmitter {
public:
  TextDiagnosticEmitter(DiagnosticOutputStream &OS, const LangOptions &LangOpts,
                        const DiagnosticOptions &DiagOpts);
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

class TextDiagnosticPrinter final : public DiagnosticClient {

  std::unique_ptr<TextDiagnosticEmitter> emitter;

  /// A string to prefix to error messages.
  std::string Prefix;

public:
  TextDiagnosticPrinter(DiagnosticOutputStream &OS, const LangOptions &LO,
                        const DiagnosticOptions &DiagOpts);
  ~TextDiagnosticPrinter() override;

public:
  TextDiagnosticEmitter &GetEmitter() { return *emitter; }
  /// SetPrefix - Set the diagnostic printer prefix string, which will be
  /// printed at the start of any diagnostics. If empty, no prefix string is
  /// used.
  void SetPrefix(std::string Value) { Prefix = std::move(Value); }

  /// Reset the TextDiagnosticEmitter
  void Reset();

  void HandleDiagnostic(DiagnosticLevel Level,
                        const DiagnosticInfo &Info) override;
};

} // namespace diags

} // namespace stone

#endif