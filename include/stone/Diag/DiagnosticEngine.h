#ifndef STONE_DIAG_DIAGNOSTIC_ENGINE_H
#define STONE_DIAG_DIAGNOSTIC_ENGINE_H

#include "stone/Diag/DiagnosticClient.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace llvm {
class Error;
class raw_ostream;
} // namespace llvm

namespace stone {
class LangOptions;

namespace diags {

class InFlightDiagnostic;
class DiagnosticClient;

struct DiagnosticStorage final {

  llvm::SmallVector<DiagnosticArgument, 3> Args;
  llvm::SmallVector<CharSrcRange, 2> Ranges;
  llvm::SmallVector<FixIt, 2> FixIts;

  DiagnosticStorage() = default;
};

class DiagnosticState final {
  /// Whether we should continue to emit diagnostics, even after a
  /// fatal error
  // bool showDiagnosticsAfterFatalError = false;

  // /// Don't emit any warnings
  // bool suppressWarnings = false;

  // /// Don't emit any remarks
  // bool suppressRemarks = false;

  // /// Emit all warnings as errors
  // bool warningsAsErrors = false;

  // /// Whether a fatal error has occurred
  // bool fatalErrorOccurred = false;

  // /// Whether any error diagnostics have been emitted.
  // bool anyErrorOccurred = false;

  // /// Track the previous emitted Behavior, useful for notes
  // DiagnosticBehavior previousBehavior = DiagnosticBehavior::Unspecified;

  // /// Track which diagnostics should be ignored.
  // llvm::BitVector ignoredDiagnostics;

  friend class DiagnosticStateRAII;

public:
  DiagnosticState();

  /// Figure out the Behavior for the given diagnostic, taking current
  /// state such as fatality into account.
  // DiagnosticBehavior determineBehavior(const Diagnostic &diag);

  // bool hadAnyError() const { return anyErrorOccurred; }
  // bool hasFatalErrorOccurred() const { return fatalErrorOccurred; }

  // void setShowDiagnosticsAfterFatalError(bool val = true) {
  //   showDiagnosticsAfterFatalError = val;
  // }
  // bool getShowDiagnosticsAfterFatalError() {
  //   return showDiagnosticsAfterFatalError;
  // }

  // /// Whether to skip emitting warnings
  // void setSuppressWarnings(bool val) { suppressWarnings = val; }
  // bool getSuppressWarnings() const { return suppressWarnings; }

  // /// Whether to skip emitting remarks
  // void setSuppressRemarks(bool val) { suppressRemarks = val; }
  // bool getSuppressRemarks() const { return suppressRemarks; }

  // /// Whether to treat warnings as errors
  // void setWarningsAsErrors(bool val) { warningsAsErrors = val; }
  // bool getWarningsAsErrors() const { return warningsAsErrors; }

  // void resetHadAnyError() {
  //   anyErrorOccurred = false;
  //   fatalErrorOccurred = false;
  // }

  // /// Set whether a diagnostic should be ignored.
  // void setIgnoredDiagnostic(DiagID id, bool ignored) {
  //   ignoredDiagnostics[(unsigned)id] = ignored;
  // }

  // void swap(DiagnosticState &other) {
  //   std::swap(showDiagnosticsAfterFatalError,
  //   other.showDiagnosticsAfterFatalError); std::swap(suppressWarnings,
  //   other.suppressWarnings); std::swap(suppressRemarks,
  //   other.suppressRemarks); std::swap(warningsAsErrors,
  //   other.warningsAsErrors); std::swap(fatalErrorOccurred,
  //   other.fatalErrorOccurred); std::swap(anyErrorOccurred,
  //   other.anyErrorOccurred); std::swap(previousBehavior,
  //   other.previousBehavior); std::swap(ignoredDiagnostics,
  //   other.ignoredDiagnostics);
  // }

private:
  // Make the state movable only
  DiagnosticState(const DiagnosticState &) = delete;
  const DiagnosticState &operator=(const DiagnosticState &) = delete;

  DiagnosticState(DiagnosticState &&) = default;
  DiagnosticState &operator=(DiagnosticState &&) = default;
};

/// DiagnosticRenderer in clang
class DiagnosticEngine final {

  friend class DiagnosticInfo;
  friend class InFlightDiagnostic;
  friend class DiagnosticErrorTrap;
  friend class InFlightPartialDiagnostic;

  // Treat fatal errors like errors.
  bool FatalsAsError = false;

  // Suppress all diagnostics.
  bool SuppressAllDiagnostics = false;

  // Elide common types of templates.
  bool ElideType = true;

  // Print a tree when comparing templates.
  bool PrintTemplateTree = false;

  // Color printing is enabled.
  bool ShowColors = false;

  // Which overload candidates to show.
  // OverloadsShown ShowOverloads = Ovl_All;

  // With Ovl_Best, the number of overload candidates to show when we encounter
  // an error.
  //
  // The value here is the number of candidates to show in the first nontrivial
  // error.  Future errors may show a different number of candidates.
  unsigned TotalOverloadsToShow = 32;

  // Cap of # errors emitted, 0 -> no limit.
  unsigned ErrorLimit = 0;

  // Cap on depth of template backtrace stack, 0 -> no limit.
  unsigned TemplateBacktraceLimit = 0;

  // Cap on depth of constexpr evaluation backtrace stack, 0 -> no limit.
  unsigned ConstexprBacktraceLimit = 0;

  /// Sticky flag set to \c true when an error is emitted.
  bool ErrorOccurred;

  /// Sticky flag set to \c true when an "uncompilable error" occurs.
  /// I.e. an error that was not upgraded from a warning by -Werror.
  bool UncompilableErrorOccurred;

  /// Sticky flag set to \c true when a fatal error is emitted.
  bool FatalErrorOccurred;

  /// Indicates that an unrecoverable error has occurred.
  bool UnrecoverableErrorOccurred;

  /// Counts for DiagnosticErrorTrap to check whether an error occurred
  /// during a parsing section, e.g. during parsing a function.
  unsigned TrapTotalErrorsOccurred;
  unsigned TrapTotalUnrecoverableErrorsOccurred;

  /// The level of the last diagnostic emitted.
  ///
  /// This is used to emit continuation diagnostics with the same level as the
  /// diagnostic that they follow.
  DiagnosticLevel LastDiagLevel;

  /// Totalber of warnings reported
  unsigned TotalWarnings;

  /// Totalber of errors reported
  unsigned TotalErrors;

  SrcMgr &SM;

  DiagnosticOptions &DiagOpts;
  /// The diagnostic consumer(s) that will be responsible for actually
  /// emitting diagnostics.
  llvm::SmallVector<DiagnosticClient *, 2> Clients;

  class Diagnosis final {

    friend DiagnosticEngine;
    friend class InFlightDiagnostic;

    DiagID ID;
    SrcLoc Loc;
    llvm::SmallVector<DiagnosticArgument, 3> Args;
    llvm::SmallVector<CharSrcRange, 2> Ranges;
    llvm::SmallVector<FixIt, 2> FixIts;
    DiagnosticLevel LevelLimit = DiagnosticLevel::None;

  public:
    Diagnosis(DiagID ID, SrcLoc Loc, ArrayRef<DiagnosticArgument> Args)
        : ID(ID), Args(Args.begin(), Args.end()) {}

    Diagnosis(DiagID ID, SrcLoc Loc) : Diagnosis(ID, Loc, {}) {}

    Diagnosis(DiagID ID) : Diagnosis(ID, SrcLoc(), {}) {}

  public:
    DiagID GetID() const { return ID; }
    llvm::ArrayRef<DiagnosticArgument> GetArgs() const { return Args; }
    llvm::ArrayRef<CharSrcRange> GetRanges() const { return Ranges; }
    llvm::ArrayRef<FixIt> GetFixIts() const { return FixIts; }

    SrcLoc GetLoc() const { return Loc; }
    void SetLoc(SrcLoc loc) { Loc = loc; }

    DiagnosticLevel GetLevelLimit() const { return LevelLimit; }
    // void SetDecl(const class Decl *decl) { Decl = decl; }
    void SetLevelLimit(DiagnosticLevel limit) { LevelLimit = limit; }

    void AddRange(CharSrcRange R) { Ranges.push_back(R); }
    // Avoid copying the fix-it text more than necessary.
    void AddArg(DiagnosticArgument &&A) { Args.push_back(std::move(A)); }
    // Avoid copying the fix-it text more than necessary.
    void AddFixIt(FixIt &&F) { FixIts.push_back(std::move(F)); }
  };

  DiagID ActiveDiagID;
  SrcLoc ActiveDiagLoc;

  std::optional<Diagnosis> ActiveDiagnosis;

private:
  DiagnosticLevel GetDiagnosticLevel(DiagID ID, SrcLoc) const;

  // DiagnosticSeverity GetDiagnosticSeverity(DiagID ID, SrcLoc) const;

public:
  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;

  explicit DiagnosticEngine(SrcMgr &SM, DiagnosticOptions &DiagOpts);
  ~DiagnosticEngine();

public:
  void AddClient(DiagnosticClient *client);

  /// Remove a specific DiagnosticConsumer.
  void RemoveClient(DiagnosticClient *client) {
    Clients.erase(std::remove(Clients.begin(), Clients.end(), client));
  }

  /// Return the current diagnostic client along with ownership of that
  /// client.
  std::vector<DiagnosticClient *> TakeClients();
  llvm::ArrayRef<DiagnosticClient *> GetClients() const { return Clients; }

  void Clear(bool soft = false);

  // DiagIDContext &GetDiagIDContext() { return diagIDContext; }

  InFlightDiagnostic Diagnose(DiagID NextDiagID);
  InFlightDiagnostic Diagnose(DiagID NextDiagID, SrcLoc NextDiagLoc);
  InFlightDiagnostic Diagnose(DiagID NextDiagID, SrcLoc NextDiagLoc,
                              llvm::ArrayRef<DiagnosticArgument> args);
  InFlightDiagnostic Diagnose(const Diagnosis &D);

  /// Determine whethere there is already a diagnostic in flight.
  bool IsInFlightDiagnostic() const {
    return ActiveDiagID != std::numeric_limits<DiagID>::max();
  }

  DiagID GetActiveDiagID() const { return ActiveDiagID; }
  SrcLoc GetActiveDiagLoc() const { return ActiveDiagLoc; }

  bool HasActiveDiagnosis() { return !ActiveDiagnosis;}

  /// Get from diag options bool ShouldShowColors() { return }

  DiagnosticKind DeclaredDiagnosticKindForDiagID(const DiagID ID);

  llvm::StringRef GetDiagnosticStringForDiagID(const DiagID ID,
                                               bool printDiagnosticNames);

  static llvm::StringRef GetDiagnosticIDStringForDiagID(const DiagID ID);

  DiagID GetCustomDiagID(DiagnosticLevel Level,
                         DiagnosticStringFormatter StringFormatter);

  /// Generate DiagnosticInfo for a Diagnostic to be passed to consumers.
  // std::optional<DiagnosticInfo>
  // CreeateDiagnosticInfoForDiagnostic(const Diagnostic &diagnostic);

  /// Given a diagnostic ID, return a description of the issue.
  llvm::StringRef GetDescriptionForDiagID(DiagID ID) const;

  bool FlushActiveDiagnostic(bool Force = false);

public:
  void FinishProcessing();

public:
  /// Get the set of all diagnostic IDs.
  static llvm::ArrayRef<DiagID> GetAllDiagnostics(DiagnosticKind Kind);

private:

  /// ProcessDiag
  /// Used to report a diagnostic that is finally fully formed.
  ///
  /// \returns \c true if the diagnostic was emitted, \c false if it was
  /// suppressed.
  bool FinishDiagnostic(DiagnosticEngine &Diag) const;

  /// Used to emit a diagnostic that is finally fully formed,
  /// ignoring suppression.
  // void EmitDiagnsotic(DiagnosticLevel Level) const;
};

// class DiagnosticStateRAII {
//     llvm::SaveAndRestore<DiagnosticLevel> previousLevel;

//   public:
//     DiagnosticStateRAII(DiagnosticEngine &diags)
//       : previousBehavior(diags.state.previousBehavior) {}

//     ~DiagnosticStateRAII() {}
//   };

//   class BufferIndirectlyCausingDiagnosticRAII {
//   private:
//     DiagnosticEngine &Diags;
//   public:
//     BufferIndirectlyCausingDiagnosticRAII(const SourceFile &SF);
//     ~BufferIndirectlyCausingDiagnosticRAII() {
//       Diags.resetBufferIndirectlyCausingDiagnostic();
//     }
//   };

class DiagnosticErrorTrap final {

  DiagnosticEngine &DE;
  unsigned TotalErrors;
  unsigned TotalUnrecoverableErrors;

public:
  explicit DiagnosticErrorTrap(DiagnosticEngine &DE) : DE(DE) { reset(); }

  /// Determine whether any errors have occurred since this
  /// object instance was created.
  bool HasErrorOccurred() const {
    return DE.TrapTotalErrorsOccurred > TotalErrors;
  }

  /// Determine whether any unrecoverable errors have occurred since this
  /// object instance was created.
  bool HasUnrecoverableErrorOccurred() const {
    return DE.TrapTotalUnrecoverableErrorsOccurred > TotalUnrecoverableErrors;
  }

  /// Set to initial state of "no errors occurred".
  void reset() {
    TotalErrors = DE.TrapTotalErrorsOccurred;
    TotalUnrecoverableErrors = DE.TrapTotalUnrecoverableErrorsOccurred;
  }
};
class StreamingDiagnostic {

protected:
  // Provides access to DiagnosticStorage
  mutable DiagnosticEngine *DE = nullptr;

protected:
  StreamingDiagnostic() = default;

  /// Construct with an external storage not owned by itself. The allocator
  /// is a null pointer in this case.
  explicit StreamingDiagnostic(DiagnosticEngine *DE) : DE(DE) {}

public:
};

class InFlightDiagnostic : public StreamingDiagnostic {

  friend class DiagnosticEngine;
  friend class PartialInFlightDiagnostic;

  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj == 0)),
  // but LLVM is not currently smart enough to eliminate the null check that
  // Emit() would end up with if we used that as our status variable.
  mutable bool IsActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool IsForceEmit = false;

public:
  InFlightDiagnostic() = default;

  explicit InFlightDiagnostic(DiagnosticEngine *DE)
      : StreamingDiagnostic(DE), IsActive(true) {

    // assert(diagObj && "DiagnosticBuilder requires a valid
    // DiagnosticsEngine!"); assert(DiagStorage &&
    //        "DiagnosticBuilder requires a valid DiagnosticStorage!");

    // DiagStorage->TotalDiagArgs = 0;
    // DiagStorage->DiagRanges.clear();
    // DiagStorage->FixItHints.clear();
  }
  ~InFlightDiagnostic() { FlushActiveDiagnostic(); }

public:
  /// Flush the active diagnostic to the diagnostic output engine.
  void FlushActiveDiagnostic();
  void Clear() {
    IsActive = false;
    IsForceEmit = false;
    DE = nullptr;
  }

public:
  InFlightDiagnostic &FixItReplace(SrcRange R, llvm::StringRef Str);

  /// Add a token-based range to the currently-active diagnostic.
  InFlightDiagnostic &Highlight(SrcRange R);

  /// Add a character-based range to the currently-active diagnostic.
  InFlightDiagnostic &HighlightChars(SrcLoc Start, SrcLoc End);
};

class StoredDiagnostic {

  DiagID ID;

  DiagnosticLevel Level;
  // FullSourceLoc Loc;
  std::string Message;
  // std::vector<CharSourceRange> Ranges;

  std::vector<FixIt> FixIts;

public:
  DiagID GetDiagID() { return ID; }
};

struct TemplateDiffTypes {};

/// ProcessWarningOptions - Initialize the diagnostic client and process the
/// warning options specified on the command line.
// void ProcessWarningOptions(DiagnosticEngine &Diags,
//                            const DiagnosticOptions &Opts,
//                            bool ReportDiags = true);
// void EscapeStringForDiagnostic(llvm::StringRef Str,
// llvm::SmallVectorImpl<char> &OutStr);

} // namespace diags
} // namespace stone

#endif