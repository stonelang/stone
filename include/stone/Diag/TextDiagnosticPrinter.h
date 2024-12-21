#ifndef STONE_DIAG_DIAGNOSTIC_PRINTER_H
#define STONE_DIAG_DIAGNOSTIC_PRINTER_H

#include "stone/Basic/LLVM.h"
#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEmitter.h"

#include <memory>

namespace stone {
namespace diags {

class TextDiagnosticEmitter final : public DiagnosticEmitter {

  // void EmitSnippetAndCaret(FullSourceLoc Loc, DiagnosticsEngine::Level Level,
  //                          SmallVectorImpl<CharSourceRange> &Ranges,
  //                          ArrayRef<FixItHint> Hints);

  void EmitSnippet(StringRef SourceLine, unsigned MaxLineNoDisplayWidth,
                   unsigned LineNo);

  // void EmitParseableFixits(llvm::ArrayRef<FixItHint> fixIts, const
  // SourceManager &SM);

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

public:
  /// Print the diagonstic level to a raw_ostream.
  ///
  /// This is a static helper that handles colorizing the level and formatting
  /// it into an arbitrary output stream. This is used internally by the
  /// TextDiagnostic emission code, but it can also be used directly by
  /// consumers that don't have a source manager or other state that the full
  /// TextDiagnostic logic requires.
  static void PrintDiagnosticLevel(DiagnosticOutputStream &OS,
                                   DiagnosticLevel DiagLevel);

  /// Pretty-print a diagnostic message to a raw_ostream.
  ///
  /// This is a static helper to handle the line wrapping, colorizing, and
  /// rendering of a diagnostic message to a particular ostream. It is
  /// publicly visible so that clients which do not have sufficient state to
  /// build a complete TextDiagnostic object can still get consistent
  /// formatting of their diagnostic messages.
  ///
  /// \param OS Where the message is printed
  /// \param IsSupplemental true if this is a continuation note diagnostic
  /// \param Message The text actually printed
  /// \param CurrentColumn The starting column of the first line, accounting
  ///                      for any prefix.
  /// \param Columns The number of columns to use in line-wrapping, 0 disables
  ///                all line-wrapping.
  /// \param ShowColors Enable colorizing of the message.
  static void PrintDiagnosticMessage(DiagnosticOutputStream &OS,
                                     bool IsSupplemental, StringRef Message,
                                     unsigned CurrentColumn, unsigned Columns);
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