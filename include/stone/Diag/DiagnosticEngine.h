#ifndef STONE_DIAG_DIAGNOSTIC_ENGINE_H
#define STONE_DIAG_DIAGNOSTIC_ENGINE_H

#include "stone/Diag/DiagnosticAllocation.h"
#include "stone/Diag/DiagnosticClient.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/SaveAndRestore.h"

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

class Diagnostic;
class InFlightDiagnostic;
class DiagnosticClient;

class DiagnosticState final {

  friend class DiagnosticEngine;

  // Suppress all diagnostics.
  bool suppressAllDiagnostics = false;

  // Color printing is enabled.
  bool showColors = false;

  /// Whether we should continue to emit diagnostics, even after a
  /// fatal error
  bool showDiagnosticsAfterFatalError = false;

  /// Don't emit any warnings
  bool suppressWarnings = false;

  /// Don't emit any remarks
  bool suppressRemarks = false;

  /// Emit all warnings as errors
  bool warningsAsErrors = false;

  /// Whether a fatal error has occurred
  bool fatalErrorOccurred = false;

  /// Whether any error diagnostics have been emitted.
  bool anyErrorOccurred = false;

  /// Sticky flag set to \c true when an "uncompilable error" occurs.
  /// I.e. an error that was not upgraded from a warning by -Werror.
  bool uncompilableErrorOccurred = false;

  /// Indicates that an unrecoverable error has occurred.
  bool unrecoverableErrorOccurred = false;

  // Treat fatal errors like errors.
  bool treatFatalErrorsLikeErrors = false;

  /// Track the previous emitted Behavior, useful for notes
  DiagnosticLevel previousLevel = DiagnosticLevel::None;

  /// Track which diagnostics should be ignored.
  llvm::BitVector ignoredDiagnostics;

  /// Track which diagnostics should be ignored.
  llvm::BitVector errorDiagnostics;

  /// Track which diagnostics should be ignored.
  llvm::BitVector warningDiagnostics;

  friend class DiagnosticStateRAII;

public:
  DiagnosticState();

  /// Figure out the Level for the given diagnostic, taking current
  /// state such as fatality into account.
  DiagnosticLevel ComputeDiagnosticLevel(const Diagnostic *diag);

  /// Whether to skip emitting warnings
  void SetSuppressAllDiagnostics(bool val) { suppressAllDiagnostics = val; }
  bool GetSuppressAllDiagnostics() const { return suppressAllDiagnostics; }

  /// Whether to skip emitting warnings
  void SetShowColors(bool val) { showColors = val; }
  bool GetShowColors() const { return showColors; }

  bool HadAnyError() const { return anyErrorOccurred; }
  bool HasFatalErrorOccurred() const { return fatalErrorOccurred; }

  void SetShowDiagnosticsAfterFatalError(bool val = true) {
    showDiagnosticsAfterFatalError = val;
  }
  bool GetShowDiagnosticsAfterFatalError() {
    return showDiagnosticsAfterFatalError;
  }

  // /// Whether to skip emitting warnings
  void SetSuppressWarnings(bool val) { suppressWarnings = val; }
  bool GetSuppressWarnings() const { return suppressWarnings; }

  // /// Whether to skip emitting remarks
  void SetSuppressRemarks(bool val) { suppressRemarks = val; }
  bool getSuppressRemarks() const { return suppressRemarks; }

  // /// Whether to treat warnings as errors
  void SetWarningsAsErrors(bool val) { warningsAsErrors = val; }
  bool GetWarningsAsErrors() const { return warningsAsErrors; }

  void ResetHadAnyError() {
    anyErrorOccurred = false;
    fatalErrorOccurred = false;
    errorDiagnostics.reset();
  }

  /// Set whether a diagnostic should be ignored.
  void SetIgnoreDiagnostic(DiagID ID, bool ignored) {
    ignoredDiagnostics[(unsigned)ID] = ignored;
  }

  void TrackWarning(DiagID ID) { warningDiagnostics.push_back((unsigned)ID); }
  unsigned GetTotalWarnings() { return warningDiagnostics.size(); }

  void TrackError(DiagID ID) { errorDiagnostics.push_back((unsigned)ID); }
  unsigned GetTotalErrors() { return errorDiagnostics.size(); }

  void swap(DiagnosticState &other) {
    std::swap(showDiagnosticsAfterFatalError,
              other.showDiagnosticsAfterFatalError);
    std::swap(suppressWarnings, other.suppressWarnings);
    std::swap(suppressRemarks, other.suppressRemarks);
    std::swap(warningsAsErrors, other.warningsAsErrors);
    std::swap(fatalErrorOccurred, other.fatalErrorOccurred);
    std::swap(anyErrorOccurred, other.anyErrorOccurred);
    std::swap(previousLevel, other.previousLevel);
    std::swap(ignoredDiagnostics, other.ignoredDiagnostics);
    std::swap(errorDiagnostics, other.errorDiagnostics);
    std::swap(warningDiagnostics, other.warningDiagnostics);
    std::swap(showColors, other.showColors);
    std::swap(suppressAllDiagnostics, other.suppressAllDiagnostics);
  }

private:
  // Make the state movable only
  DiagnosticState(const DiagnosticState &) = delete;
  const DiagnosticState &operator=(const DiagnosticState &) = delete;

  DiagnosticState(DiagnosticState &&) = default;
  DiagnosticState &operator=(DiagnosticState &&) = default;
};

enum class DiagnosticStage {
  None = 0,
  Active,
  Flushed,
  Emitted,
};

class Diagnostic : public DiagnosticAllocation<Diagnostic> {
  friend class DiagnosticEngine;
  friend class InFlightDiagnostic;

protected:
  DiagID ID;
  SrcLoc Loc;

  DiagnosticStage Stage = DiagnosticStage::None;
  // DiagnosticState State;

  llvm::SmallVector<DiagnosticArgument, 3> Args;
  llvm::SmallVector<CharSrcRange, 2> Ranges;
  llvm::SmallVector<FixIt, 2> FixIts;
  DiagnosticLevel Level = DiagnosticLevel::None;

  void SetLoc(SrcLoc loc) { Loc = loc; }
  void SetLevel(DiagnosticLevel level) { Level = level; }

  void AddRange(CharSrcRange R) { Ranges.push_back(R); }
  // Avoid copying the fix-it text more than necessary.
  void AddArg(DiagnosticArgument &&A) { Args.push_back(std::move(A)); }
  // Avoid copying the fix-it text more than necessary.
  void AddFixIt(FixIt &&F) { FixIts.push_back(std::move(F)); }

  /// Set the diagnostic stage
  void SetStage(DiagnosticStage S) { Stage = S; }

public:
  // All constructors are intentionally implicit.
  template <typename... ArgTypes>
  Diagnostic(Diag<ArgTypes...> ID,
             typename detail::PassArgument<ArgTypes>::type... VArgs)
      : ID(ID.ID) {
    DiagnosticArgument DiagArgs[] = {DiagnosticArgument(0),
                                     std::move(VArgs)...};
    Args.append(DiagArgs + 1, DiagArgs + 1 + sizeof...(VArgs));
  }

  Diagnostic(DiagID ID, llvm::ArrayRef<DiagnosticArgument> Args)
      : ID(ID), Args(Args.begin(), Args.end()) {}

  /// Evaluates true when this object stores a diagnostic.
  // explicit operator bool() const { return !Diag->Message.empty(); }

public:
  DiagID GetID() const { return ID; }
  DiagnosticStage GetStage() { return Stage; }
  llvm::ArrayRef<DiagnosticArgument> GetArgs() const { return Args; }
  llvm::ArrayRef<CharSrcRange> GetRanges() const { return Ranges; }
  llvm::ArrayRef<FixIt> GetFixIts() const { return FixIts; }
  SrcLoc GetLoc() const { return Loc; }
  DiagnosticLevel GetLevel() const { return Level; }
  //  bool IsFromCache() { return FromCache; }

public:
  static Diagnostic *Create(DiagnosticEngine &DE, DiagID ID);
  static Diagnostic *Create(DiagnosticEngine &DE, SrcLoc Loc, DiagID ID);

  // static Diagnostic *Create(DiagnosticEngine &DE, DiagID ID, SrcLoc Loc,
  //                           ArrayRef<DiagnosticArgument> Args);

  // static Diagnostic *Create(DiagnosticEngine &DE, DiagID ID, SrcLoc Loc,
  //                           ArrayRef<DiagnosticArgument> Args,
  //                           ArrayRef<FixIt> FixIts);
};

/// Primarily builds out the Diagnostic with fixit decorations.
class InFlightDiagnostic final {
  friend class DiagnosticEngine;
  DiagnosticEngine *DE;
  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj ==
  // 0)), but LLVM is not currently smart enough to eliminate the null check
  // that Emit() would end up with if we used that as our status variable.
  mutable bool IsActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool IsForceEmit = false;
  /// Create a new in-flight diagnostic.
  ///
  /// This constructor is only available to the DiagnosticEngine.
  InFlightDiagnostic(DiagnosticEngine &Engine) : DE(&Engine), IsActive(true) {}

  InFlightDiagnostic(const InFlightDiagnostic &) = delete;
  InFlightDiagnostic &operator=(const InFlightDiagnostic &) = delete;
  InFlightDiagnostic &operator=(InFlightDiagnostic &&) = delete;

  void Clear() {
    IsActive = false;
    IsForceEmit = false;
    DE = nullptr;
  }

public:
  /// Create an active but unattached in-flight diagnostic.
  ///
  /// The resulting diagnostic can be used as a dummy, accepting the
  /// syntax to add additional information to a diagnostic without
  /// actually emitting a diagnostic.
  InFlightDiagnostic() : DE(0), IsActive(true) {}

  /// Transfer an in-flight diagnostic to a new object, which is
  /// typically used when returning in-flight diagnostics.
  InFlightDiagnostic(InFlightDiagnostic &&Other)
      : DE(Other.DE), IsActive(Other.IsActive) {
    Other.IsActive = false;
  }

  /// Flush the active diagnostic to the diagnostic output engine.
  void FlushActiveDiagnostic();

  /// Forces the diagnostic to be emitted.
  const InFlightDiagnostic &SetForceEmitDiagnostic() const {
    IsForceEmit = true;
    return *this;
  }

  /// Prevent the diagnostic from behaving more severely than \p limit. For
  /// instance, if \c DiagnosticBehavior::Warning is passed, an error will be
  /// emitted as a warning, but a note will still be emitted as a note.
  InFlightDiagnostic &SetDiagnosticLevel(DiagnosticLevel Level);

public:
  InFlightDiagnostic &FixItReplace(SrcRange R, llvm::StringRef Str);

  /// Add a token-based range to the currently-active diagnostic.
  InFlightDiagnostic &Highlight(SrcRange R);

  /// Add a character-based range to the currently-active diagnostic.
  InFlightDiagnostic &HighlightChars(SrcLoc StartLoc, SrcLoc EndLoc);

  /// Add an argument to the diagnostic
  InFlightDiagnostic &AddDiagnosticArgument(const DiagnosticArgument argument);

public:
  /// May be faster.
  friend const InFlightDiagnostic &operator<<(const InFlightDiagnostic &ID,
                                              int I) {
    return ID;
  }
  friend const InFlightDiagnostic &operator<<(const InFlightDiagnostic &ID,
                                              llvm::StringRef Text) {
    return ID;
  }
};

/// DiagnosticRenderer in clang
class DiagnosticEngine final {

  friend class Diagnostic;
  friend class InFlightDiagnostic;
  friend class DiagnosticErrorTrap;
  friend class DiagnosticStateRAII;

  /// The allocator used to create Diagnostics -- released only when the
  /// DiagnosticEngine is destroyed
  mutable llvm::BumpPtrAllocator allocator;

  // Elide common types of templates.
  bool ElideType = true;

  // Print a tree when comparing templates.
  bool PrintTemplateTree = false;

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

  /// Counts for DiagnosticErrorTrap to check whether an error occurred
  /// during a parsing section, e.g. during parsing a function.
  unsigned TrapTotalErrorsOccurred;
  unsigned TrapTotalUnrecoverableErrorsOccurred;

  /// Tracks diagnostic behaviors and state
  DiagnosticState state;

  SrcMgr &SM;

  DiagnosticOptions &DiagOpts;
  /// The diagnostic consumer(s) that will be responsible for actually
  /// emitting diagnostics.
  llvm::SmallVector<DiagnosticClient *, 2> Clients;

  /// The active dianostics
  Diagnostic *ActiveDiagnostic = nullptr;
  Diagnostic *GetActiveDiagnostic() { return ActiveDiagnostic; }

  /// All diagnostics that have are no longer active but have not yet
  /// been emitted due to an open transaction.
  llvm::SmallVector<Diagnostic, 4> TentativeDiagnostics;

  /// Requested by InFlightDiagnostic
  void FlushActiveDiagnostic(bool ForceEmit = false);

  /// Flush a new diagnostic, which will either be emitted, or added to an
  /// active transaction.
  void FlushActiveDiagnostic(const Diagnostic *diagnostic);



  /// Used to emit a diagnostic that is finally fully formed,
  /// ignoring suppression.
  void EmitDiagnostic(const Diagnostic *diagnostic);

  /// Clear any tentative diagnostics.
  void ClearTentativeDiagnostics();

  /// Send all tentative diagnostics to all diagnostic consumers and
  /// delete them.
  void EmitTentativeDiagnostics();

  /// Send all diagnostics to the queue
  void QueueDiagnostic(const Diagnostic *diagnostic);

public:
  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;

  explicit DiagnosticEngine(SrcMgr &SM, DiagnosticOptions &DiagOpts);
  ~DiagnosticEngine();
  void Clear(bool soft = false);

  bool FinishProcessing();

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
  bool HasClients() const { return Clients.size() > 0; }

public:
  InFlightDiagnostic Diagnose(DiagID NextDiagID);
  InFlightDiagnostic Diagnose(SrcLoc Loc, DiagID NextDiagID);

  // InFlightDiagnostic Diagnose(DiagID NextDiagID, SrcLoc NextDiagLoc);
  // InFlightDiagnostic Diagnose(DiagID NextDiagID, SrcLoc NextDiagLoc,
  //                               llvm::ArrayRef<DiagnosticArgument> args);

  InFlightDiagnostic Diagnose(const Diagnostic *D);

  bool HasActiveDiagnsotic() const { return ActiveDiagnostic != nullptr; }
  /// Determine whethere there is already a diagnostic in flight.
  bool IsInFlightDiagnostic() const { return HasActiveDiagnsotic(); }

public:
  DiagnosticKind DeclaredDiagnosticKindForDiagID(const DiagID ID);

  llvm::StringRef
  GetDiagnosticStringForDiagID(const DiagID ID,
                               bool printDiagnosticNames = false);

  static llvm::StringRef GetDiagnosticIDStringForDiagID(const DiagID ID);

  DiagID GetCustomDiagID(DiagnosticLevel Level,
                         DiagnosticStringFormatter StringFormatter);

  /// Generate DiagnosticInfo for a Diagnostic to be passed to consumers.
  std::optional<DiagnosticImpl>
  ConstructDiagnosticImpl(const Diagnostic *diagnostic);

  /// Given a diagnostic ID, return a description of the issue.
  llvm::StringRef GetDescriptionForDiagID(DiagID ID) const;

  /// Get the set of all diagnostic IDs.
  static llvm::ArrayRef<DiagID> GetDiagnostics(DiagnosticKind Kind);

  /// Get the total amount of diagnostics -- this may be need for creating
  /// custom diags
  static unsigned GetTotalDiagnostics();

  /// Format the given diagnostic text and place the result in the given
  /// buffer.
  static void FormatDiagnosticText(
      llvm::raw_ostream &Out, StringRef Text, SrcMgr &SM,
      ArrayRef<DiagnosticArgument> Args,
      DiagnosticFormatOptions FormatOpts = DiagnosticFormatOptions());

  static void FormatDiagnosticText(
      llvm::raw_ostream &Out, const char *StrPtr, const char *EndPtr,
      SrcMgr &SM, ArrayRef<DiagnosticArgument> Args,
      DiagnosticFormatOptions FormatOpts = DiagnosticFormatOptions());

private:
  DiagnosticLevel GetDiagnosticLevel(DiagID ID, SrcLoc) const;

public:
  llvm::BumpPtrAllocator &GetAllocator() { return allocator; }
  /// Allocate - Allocate memory from the Driver bump pointer.
  void *Allocate(unsigned long Bytes, unsigned Alignment = 8) const {
    if (Bytes == 0) {
      return nullptr;
    }
    return allocator.Allocate(Bytes, Alignment);
  }
};

/// Remember details about the state of a diagnostic engine and restore them
/// when the object is destroyed.
///
/// Diagnostic engines contain state about the most recent diagnostic emitted
/// which influences subsequent emissions; in particular, if you try to emit
/// a note and the previous diagnostic was ignored, the note will be ignored
/// too. This can be a problem in code structured like:
///
///     D->diagnose(diag::an_error);
///     if (conditionWhichMightEmitDiagnostics())
///        D->diagnose(diag::a_note); // might be affected by diagnostics from
///                                   // conditionWhichMightEmitDiagnostics()!
///
/// To prevent this, functions which are called for their return values but
/// may emit diagnostics as a side effect can use \c DiagnosticStateRAII to
/// ensure that their changes to diagnostic engine state don't leak out and
/// affect the caller's diagnostics.
class DiagnosticStateRAII {
  llvm::SaveAndRestore<DiagnosticLevel> previousLevel;

public:
  DiagnosticStateRAII(DiagnosticEngine &DE)
      : previousLevel(DE.state.previousLevel) {}

  ~DiagnosticStateRAII() {}
};

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
} // namespace stone

#endif