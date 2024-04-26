#ifndef STONE_DIAG_DIAGNOSTICENGINE_H
#define STONE_DIAG_DIAGNOSTICENGINE_H

#include "stone/Basic/LangOptions.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Version.h"
#include "stone/Support/Diagnostic.h"
#include "stone/Support/DiagnosticConsumer.h"
#include "stone/Support/DiagnosticFormatter.h"
#include "stone/Support/DiagnosticLocalization.h"

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
class DiagnosticConsumer;
class LangOptions;
class Version;
class LexerBase;
class SavedDiagnostic;
class DiagnosticState;

class PrintingPolicy {
public:
};
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
  friend class DiagnosticFixer;
  friend class DiagnosticEngine;

  DiagnosticFixer fixer;
  DiagnosticEngine *de;
  LexerBase *lexerBase;

  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj == 0)),
  // but LLVM is not currently smart enough to eliminate the null check that
  // Emit() would end up with if we used that as our status variable.
  mutable bool isActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool isForceFlush = false;

  InFlightDiagnostic(const InFlightDiagnostic &) = delete;
  InFlightDiagnostic &operator=(const InFlightDiagnostic &) = delete;
  InFlightDiagnostic &operator=(InFlightDiagnostic &&) = delete;

public:
  InFlightDiagnostic();

  InFlightDiagnostic(DiagnosticEngine &de, LexerBase *lexerBase = nullptr);

  /// Transfer an in-flight diagnostic to a new object, which is
  /// typically used when returning in-flight diagnostics.
  InFlightDiagnostic(InFlightDiagnostic &&other)
      : de(other.de), fixer(*this), isActive(other.isActive),
        isForceFlush(other.isForceFlush), lexerBase(other.lexerBase) {
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
  DiagnosticFixer &WithFix() { return fixer; }
  DiagnosticEngine &GetDiags() { return *de; }

  /// Send the diagnostic to the DiagnosticEngine output.
  void Flush();

  /// Determine whether this diagnostic is still active.
  bool IsActive() const { return isActive; }

  bool IsForceFlush() const { return isForceFlush; }
};

class DiagnosticConsumer;
class DiagnosticEngine;

/// RAII class that suppresses diagnostics by temporarily disabling all of
/// the diagnostic consumers.
class DiagnosticSuppression {
  DiagnosticEngine &diags;
  std::vector<DiagnosticConsumer *> consumers;

  DiagnosticSuppression(const DiagnosticSuppression &) = delete;
  DiagnosticSuppression &operator=(const DiagnosticSuppression &) = delete;

public:
  explicit DiagnosticSuppression(DiagnosticEngine &diags);
  ~DiagnosticSuppression();
  static bool isEnabled(const DiagnosticEngine &diags);
};
/// Concrete class used by the front-end to report problems and issues.
///
/// This massages the diagnostics (e.g. handling things like "report warnings
/// as errors" and passes them off to the DiagnosticConsumer for reporting to
/// the user. Diagnostics is tied to one translation unit and one
/// SrcMgr.
class DiagnosticEngine final {
  friend class InFlightDiagnostic;
  friend class DiagnosticTransaction;
  friend struct DiagnosticArgument;

  SrcMgr &sm;

  /// The
  unsigned int diagnosticSeen = 0;

  /// The diagnostic consumers(s) that will be responsible for actually
  /// emitting diagnostics.
  llvm::SmallVector<DiagnosticConsumer *, 2> consumers;

  /// The current diagnostic, if there is one.
  std::optional<Diagnostic> curDiagnostic;

  /// The initial diagnostic state.
  DiagnosticState state;

  llvm::BumpPtrAllocator transactionAllocator;

  /// A set of all strings involved in current transactional chain.
  /// This is required because diagnostics are not directly emitted
  /// but rather stored until all transactions complete.
  // llvm::StringSet<llvm::BumpPtrAllocator &> transactionStrings;

  /// Diagnostic producer to handle the logic behind retrieving a localized
  /// diagnostic message.
  std::unique_ptr<DiagnosticLocalizationProducer> localization;

  /// The number of open diagnostic transactions. Diagnostics are only
  /// emitted once all transactions have closed.
  unsigned transactionCount = 0;

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
  explicit DiagnosticEngine(SrcMgr &sm);

  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;

  bool Finish();

public:
  bool HasError();
  SrcMgr &GetSrcMgr() { return sm; }
  /// Specify a limit for the number of errors we should
  /// emit before giving up.
  ///
  void Print(ColorStream &os, const PrintingPolicy *policy) const;

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
  /// Add an additional DiagnosticConsumer to receive diagnostics.
  void AddConsumer(DiagnosticConsumer &listener) {
    consumers.push_back(&listener);
  }
  /// Remove a specific DiagnosticConsumer.
  void RemoveConsumer(DiagnosticConsumer &listener) {
    consumers.erase(std::remove(consumers.begin(), consumers.end(), &listener));
  }

  /// Remove and return all \c DiagnosticConsumer.
  std::vector<DiagnosticConsumer *> TakeListeners() {
    auto result =
        std::vector<DiagnosticConsumer *>(consumers.begin(), consumers.end());
    consumers.clear();
    return result;
  }

  /// Return all \c DiagnosticConsumer.
  llvm::ArrayRef<DiagnosticConsumer *> GetListeners() const {
    return consumers;
  }

public:
  /// Generate DiagnosticMessage for a Diagnostic to be passed to consumers.
  std::optional<DiagnosticMessage>
  CreateDiagnosticMessage(const Diagnostic &diagnostic);

  // Send \c diag to all diagnostic consumers.
  void EmitDiagnostic(const Diagnostic &diagnostic);

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
                                              LexerBase *lexerBase = nullptr) {
    assert(!curDiagnostic && "Already have an active diagnostic");
    curDiagnostic = diagnostic;
    curDiagnostic->SetLoc(loc);
    return InFlightDiagnostic(*this, lexerBase);
  }

public:
  InFlightDiagnostic PrintD(SrcLoc loc, const Diagnostic diagnostic,
                            LexerBase *lexerBase = nullptr) {
    return CreateInFlightDiagnostic(loc, diagnostic, lexerBase);
  }

  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                            llvm::ArrayRef<DiagnosticArgument> args,
                            LexerBase *lexerBase = nullptr) {
    return PrintD(loc, Diagnostic(Diagnostic(diagID, args)), lexerBase);
  }

  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                            LexerBase *lexerBase = nullptr) {
    return PrintD(loc, Diagnostic(diagID, llvm::ArrayRef<DiagnosticArgument>()),
                  lexerBase);
  }
  InFlightDiagnostic PrintD(DiagID diagID, LexerBase *lexerBase = nullptr) {
    return PrintD(SrcLoc(),
                  Diagnostic(diagID, llvm::ArrayRef<DiagnosticArgument>()),
                  lexerBase);
  }

  template <typename... ArgTypes>
  InFlightDiagnostic
  PrintD(SrcLoc loc, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return PrintD(loc, Diagnostic(id, std::move(args)...));
  }

  template <typename... ArgTypes>
  InFlightDiagnostic
  PrintD(Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return PrintD(SrcLoc(), Diagnostic(id, std::move(args)...));
  }

  template <typename... ArgTypes>
  InFlightDiagnostic
  PrintD(SrcLoc loc, LexerBase *lexerBase, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return PrintD(loc, Diagnostic(id, std::move(args)...), lexerBase);
  }
};
class DiagnosticStateRAII final {
  llvm::SaveAndRestore<diag::Level> prevLevel;

public:
  DiagnosticStateRAII(DiagnosticEngine &de) : prevLevel(de.state.prevLevel) {}

  ~DiagnosticStateRAII() {}
};

class DiagnosticTransaction {
protected:
  // DiagnosticEngine &de;

  // /// How many tentative diagnostics there were when the transaction
  // /// was opened.
  // unsigned prevDiagnostics;

  // /// How many other transactions were open when this transaction was
  // /// opened.
  // unsigned depth;

  // /// Whether this transaction is currently open.
  // bool isOpen = true;

private:
  void Close() {
    // assert(isOpen && "only open transactions may be closed");
    // IsOpen = false;
    // Engine.TransactionCount--;
    // assert(Depth == Engine.TransactionCount &&
    //        "transactions must be closed LIFO");
  }

public:
  // DiagnosticTransaction(const DiagnosticTransaction &) = delete;
  // DiagnosticTransaction &operator=(const DiagnosticTransaction &) = delete;

  // explicit DiagnosticTransaction(DiagnosticEngine &de)
  //   : de(de),
  //     prevDiagnostics(de.GetTentativeDiagnosticsSize()),
  //     depth(de.GetTransactionCount()),
  //     isOpen(true)
  // {
  //   //de.transactionCount++;
  // }

  /// Abort and close this transaction and erase all diagnostics
  /// record while it was open.
  void Abort() {
    Close();
    // Engine.TentativeDiagnostics.erase(
    //   Engine.TentativeDiagnostics.begin() + PrevDiagnostics,
    //   Engine.TentativeDiagnostics.end());
  }

  /// Commit and close this transaction. If this is the top-level
  /// transaction, emit any diagnostics that were recorded while it was open.
  void Commit() {
    Close();
    // if (Depth == 0) {
    //   assert(PrevDiagnostics == 0);
    //   Engine.emitTentativeDiagnostics();
    // }
  }
};

class CompoundDiagnosticTransaction : public DiagnosticTransaction {
public:
};

class DiagnosticQueue final {
  /// The underlying diagnostic engine that the diagnostics will be emitted
  /// by.
  // DiagnosticEngine &underlyingEngine;

  // /// A temporary engine used to queue diagnostics.
  // DiagnosticEngine queueEngine;

  /// Whether the queued diagnostics should be emitted on the destruction of
  /// the queue, or whether they should be cleared.
  bool emitOnDestruction;

public:
  DiagnosticQueue(const DiagnosticQueue &) = delete;
  DiagnosticQueue &operator=(const DiagnosticQueue &) = delete;
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