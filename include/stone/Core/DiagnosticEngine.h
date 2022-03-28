#ifndef STONE_CORE_DIAGNOSTICENGINE_H
#define STONE_CORE_DIAGNOSTICENGINE_H

#include "stone/Core/Diagnostic.h"
#include "stone/Core/DiagnosticFormatter.h"
#include "stone/Core/DiagnosticListener.h"
#include "stone/Core/DiagnosticTransaction.h"
#include "stone/Core/List.h"
#include "stone/Core/Printable.h"
#include "stone/Core/SystemOptions.h"
#include "stone/Core/Version.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/SaveAndRestore.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {

class SrcMgr;
class DiagnosticEngine;
class InFlightDiagnostic;
class DiagnosticListener;
class SystemOptions;
class Version;
class SavedDiagnostic;
class DiagnosticState;
class Tokenable;

class DiagnosticState {
  /// Whether we should continue to emit diagnostics, even after a
  /// fatal error
  bool showDiagnosticsAfterFatalError = false;

  /// Don't emit any warnings
  bool suppressWarnings = false;

  /// Emit all warnings as errors
  bool warningsAsErrors = false;

  /// Whether a fatal error has occurred
  bool fatalErrorOccurred = false;

  /// Whether any error diagnostics have been emitted.
  bool anyErrorOccurred = false;

  /// Track the previous emitted Behavior, useful for notes
  diag::Level prevLevel = diag::Level::None;

  /// Track which diagnostics should be ignored.
  llvm::BitVector ignoredDiagnostics;

  friend class DiagnosticStateRAII;

public:
  DiagnosticState();

  /// Figure out the Behavior for the given diagnostic, taking current
  /// state such as fatality into account.
  diag::Level DetermineLevel(const Diagnostic &diag);

  bool HadAnyError() const { return anyErrorOccurred; }
  bool HasFatalErrorOccurred() const { return fatalErrorOccurred; }

  void SetShowDiagnosticsAfterFatalError(bool val = true) {
    showDiagnosticsAfterFatalError = val;
  }
  bool GetShowDiagnosticsAfterFatalError() {
    return showDiagnosticsAfterFatalError;
  }

  /// Whether to skip emitting warnings
  void SetSuppressWarnings(bool val) { suppressWarnings = val; }
  bool GetSuppressWarnings() const { return suppressWarnings; }

  /// Whether to treat warnings as errors
  void SetWarningsAsErrors(bool val) { warningsAsErrors = val; }
  bool GetWarningsAsErrors() const { return warningsAsErrors; }

  void ResetHadAnyError() {
    anyErrorOccurred = false;
    fatalErrorOccurred = false;
  }

  /// Set whether a diagnostic should be ignored.
  void SetIgnoredDiagnostic(DiagID diagID, bool ignored) {
    ignoredDiagnostics[(unsigned)diagID] = ignored;
  }

  void Swap(DiagnosticState &other) {
    std::swap(showDiagnosticsAfterFatalError,
              other.showDiagnosticsAfterFatalError);
    std::swap(suppressWarnings, other.suppressWarnings);
    std::swap(warningsAsErrors, other.warningsAsErrors);
    std::swap(fatalErrorOccurred, other.fatalErrorOccurred);
    std::swap(anyErrorOccurred, other.anyErrorOccurred);
    std::swap(prevLevel, other.prevLevel);
    std::swap(ignoredDiagnostics, other.ignoredDiagnostics);
  }

private:
  // Make the state movable only
  DiagnosticState(const DiagnosticState &) = delete;
  const DiagnosticState &operator=(const DiagnosticState &) = delete;

  DiagnosticState(DiagnosticState &&) = default;
  DiagnosticState &operator=(DiagnosticState &&) = default;
};

class InFlightDiagnostic {
  friend class CodeFixer;
  friend class DiagnosticEngine;

  CodeFixer fixer;
  DiagnosticEngine &de;
  Tokenable *tokenable;

  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj == 0)),
  // but LLVM is not currently smart enough to eliminate the null check that
  // Emit() would end up with if we used that as our status variable.
  mutable bool isActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool isForceFlush = false;

  InFlightDiagnostic() = default;
  InFlightDiagnostic(const InFlightDiagnostic &) = delete;
  InFlightDiagnostic &operator=(const InFlightDiagnostic &) = delete;
  InFlightDiagnostic &operator=(InFlightDiagnostic &&) = delete;

public:
  InFlightDiagnostic(DiagnosticEngine &de, Tokenable *tokenable = nullptr);

  /// Transfer an in-flight diagnostic to a new object, which is
  /// typically used when returning in-flight diagnostics.
  InFlightDiagnostic(InFlightDiagnostic &&other)
      : de(other.de), fixer(*this), isActive(other.isActive),
        isForceFlush(other.isForceFlush), tokenable(other.tokenable) {
    other.isActive = false;
    other.isForceFlush = false;
  }
  /// Emits the diagnostic.
  ~InFlightDiagnostic() {
    if (IsActive()) {
      Flush();
    }
  }

public:
  CodeFixer &WithFix() { return fixer; }
  DiagnosticEngine &GetDiagEngine() { return de; }

  /// Send the diagnostic to the DiagnosticEngine output.
  void Flush();

  /// Determine whether this diagnostic is still active.
  bool IsActive() const { return isActive; }

  bool IsForceFlush() const { return isForceFlush; }
};

/// Concrete class used by the front-end to report problems and issues.
///
/// This massages the diagnostics (e.g. handling things like "report warnings
/// as errors" and passes them off to the DiagnosticConsumer for reporting to
/// the user. Diagnostics is tied to one translation unit and one
/// SrcMgr.
class DiagnosticEngine final : public Printable {
  friend class InFlightDiagnostic;
  friend class DiagnosticTransaction;
  friend struct diag::Argument;

  /// The
  unsigned int diagnosticSeen = 0;

  /// The diagnostic listeners(s) that will be responsible for actually
  /// emitting diagnostics.
  llvm::SmallVector<DiagnosticListener *, 2> listeners;

  /// The current diagnostic, if there is one.
  llvm::Optional<Diagnostic> curDiagnostic;

  // llvm::IntrusiveRefCntPtr<DiagnosticOptions> diagOptions;
  DiagnosticOptions &diagOpts;

  SrcMgr &sm;

  /// The initial diagnostic state.
  DiagnosticState state;

  llvm::BumpPtrAllocator transactionAllocator;

  /// All diagnostics that are no longer active but have not yet
  /// been emitted due to an open transaction.
  llvm::SmallVector<Diagnostic, 4> pendingDiagnostics;

  /// Sticky flag set to \c true when an error is emitted.
  bool errorOccurred;

  /// Sticky flag set to \c true when an "unci error" occurs.
  /// I.e. an error that was not upgraded from a warning by -Werror.
  bool unciErrorOccurred;

  /// Sticky flag set to \c true when a fatal error is emitted.
  bool fatalErrorOccurred;

  /// Indicates that an unrecoverable error has occurred.
  bool unrecoverableErrorOccurred;

  /// The level of the last diagnostic emitted.
  ///
  /// This is used to emit continuation diagnostics with the same level as the
  /// diagnostic that they follow.
  diag::Level lastLevel;

  /// Number of warnings reported
  unsigned numWarnings;

  /// Number of errors reported
  unsigned numErrors;

  Version version;

  friend class InFlightDiagnostic;
  // friend class DiagnosticTransaction;
  // friend class CompoundDiagnosticTransaction;
  friend class DiagnosticStateRAII;

public:
  explicit DiagnosticEngine(DiagnosticOptions &diagOpts, SrcMgr &sm);

  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;

  void Finish();

public:
  bool HasError();
  SrcMgr &GetSrcMgr() { return sm; }

  void Print(ColorfulStream &os, const PrintingPolicy *policy) const override;

  /// Specify a limit for the number of errors we should
  /// emit before giving up.
  ///
  /// Zero disables the limit.
  void SetErrorLimit(unsigned limit) { diagOpts.errorLimit = limit; }

public:
  //==State management==//
  /// HadAnyError - return true if any *error* diagnostics have been emitted.
  bool HadAnyError() const { return state.HadAnyError(); }
  bool HasFatalErrorOccurred() const { return state.HasFatalErrorOccurred(); }

  void SetShowDiagnosticsAfterFatalError(bool val = true) {
    state.SetShowDiagnosticsAfterFatalError(val);
  }
  bool GetShowDiagnosticsAfterFatalError() {
    return state.GetShowDiagnosticsAfterFatalError();
  }
  /// Whether to skip emitting warnings
  void SetSuppressWarnings(bool val) { state.SetSuppressWarnings(val); }
  bool GetSuppressWarnings() const { return state.GetSuppressWarnings(); }

  /// Whether to treat warnings as errors
  void SetWarningsAsErrors(bool val) { state.SetWarningsAsErrors(val); }
  bool GetWarningsAsErrors() const { return state.GetWarningsAsErrors(); }

  void IgnoreDiagnostic(DiagID diagID) {
    state.SetIgnoredDiagnostic(diagID, true);
  }
  void ResetHadAnyError() { state.ResetHadAnyError(); }

  /// Grab the most-recently-added state point.
  DiagnosticState &GetDiagState() { return state; }

public:
  /// Get the actual string in the ".def" for the diagnostic
  llvm::StringRef GetDiagString(const DiagID diagID, bool printDiagnosticName);
  llvm::StringRef GetDiagIDStringByDiagID(const DiagID diagID);

public:
  /// Add an additional DiagnosticListener to receive diagnostics.
  void AddListener(DiagnosticListener &listener) {
    listeners.push_back(&listener);
  }
  /// Remove a specific DiagnosticListener.
  void RemoveListener(DiagnosticListener &listener) {
    listeners.erase(std::remove(listeners.begin(), listeners.end(), &listener));
  }

  /// Remove and return all \c DiagnosticListener.
  std::vector<DiagnosticListener *> TakeListeners() {
    auto result =
        std::vector<DiagnosticListener *>(listeners.begin(), listeners.end());
    listeners.clear();
    return result;
  }

  /// Return all \c DiagnosticListener.
  llvm::ArrayRef<DiagnosticListener *> GetListeners() const {
    return listeners;
  }

  void FlushListeners() {
    for (auto listener : listeners)
      listener->Flush();
  }

public:
  /// Generate DiagnosticEvent for a Diagnostic to be passed to listeners.
  llvm::Optional<DiagnosticEvent>
  CreateDiagnosticEvent(const Diagnostic &diagnostic);

  // Send \c diag to all diagnostic listeners.
  void EmitSpecificDiagnostic(const Diagnostic &diag);

  /// Send all tentative diagnostics to all diagnostic consumers and
  /// delete them.
  void EmitPendingDiagnostics();

  /// Emit the current diagnostic and clear the diagnostic state.
  ///
  /// \param Force Emit the diagnostic regardless of suppression settings.
  bool EmitCurrentDiagnostic(bool force = false);

  /// Flush the active diagnostic.
  void FlushCurrentDiagnostic();

  Diagnostic &GetCurrentDiagnostic() { return *curDiagnostic; }

  /// Determine whethere there is already a diagnostic in flight -- there is a
  /// better way.
  bool IsDiagnosticInFlight() {
    if (curDiagnostic) {
      return true;
    }
    return false;
  }

private:
  InFlightDiagnostic CreateInFlightDiagnostic(SrcLoc loc,
                                              const Diagnostic &diagnostic,
                                              Tokenable *tokenable = nullptr) {
    assert(!curDiagnostic && "Already have an active diagnostic");
    curDiagnostic = diagnostic;
    curDiagnostic->GetDetail().SetLoc(loc);
    return InFlightDiagnostic(*this, tokenable);
  }

public:
  InFlightDiagnostic PrintD(SrcLoc loc, const Diagnostic &diagnostic,
                            Tokenable *tokenable = nullptr) {
    return CreateInFlightDiagnostic(loc, diagnostic, tokenable);
  }

  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                            llvm::ArrayRef<diag::Argument> args,
                            Tokenable *tokenable = nullptr) {
    return PrintD(loc, Diagnostic(DiagnosticDetail(diagID, args)), tokenable);
  }

  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                            Tokenable *tokenable = nullptr) {
    return PrintD(
        loc,
        Diagnostic(DiagnosticDetail(diagID, llvm::ArrayRef<diag::Argument>())),
        tokenable);
  }
  InFlightDiagnostic PrintD(DiagID diagID, Tokenable *tokenable = nullptr) {
    return PrintD(
        SrcLoc(),
        Diagnostic(DiagnosticDetail(diagID, llvm::ArrayRef<diag::Argument>())),
        tokenable);
  }

  template <typename... ArgTypes>
  InFlightDiagnostic
  PrintD(SrcLoc loc, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return PrintD(loc, Diagnostic(DiagnosticDetail(id, std::move(args)...)));
  }

  template <typename... ArgTypes>
  InFlightDiagnostic
  PrintD(SrcLoc loc, Tokenable *tokenable, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return PrintD(loc, Diagnostic(DiagnosticDetail(id, std::move(args)...)),
                  tokenable);
  }
};
class DiagnosticStateRAII final {
  llvm::SaveAndRestore<diag::Level> prevLevel;

public:
  DiagnosticStateRAII(DiagnosticEngine &de) : prevLevel(de.state.prevLevel) {}

  ~DiagnosticStateRAII() {}
};

// class BufferIndirectlyCausingDiagnosticRAII {
// private:
//   DiagnosticEngine &Diags;
// public:
//   BufferIndirectlyCausingDiagnosticRAII(const SourceFile &SF);
//   ~BufferIndirectlyCausingDiagnosticRAII() {
//     Diags.resetBufferIndirectlyCausingDiagnostic();
//   }
// };
class SavedDiagnostic final {
  // unsigned diagIdentifier;
  // diag::Level level;
  // FullSourceLoc loc;
  // std::string message;
  // std::vector<CharSrcRange> ranges;
  // std::vector<FixHint> hints;

public:
  SavedDiagnostic() = default;
};

} // namespace stone
#endif