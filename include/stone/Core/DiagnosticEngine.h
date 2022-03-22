#ifndef STONE_CORE_DIAGNOSTICENGINE_H
#define STONE_CORE_DIAGNOSTICENGINE_H

#include "stone/Core/Diagnostic.h"
#include "stone/Core/DiagnosticEngineBase.h"
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

namespace stone {

class SrcMgr;
class DiagnosticEngine;
class InFlightDiagnostic;
class DiagnosticListener;
class SystemOptions;
class Version;
class SavedDiagnostic;
class Tokenable;

class DiagnosticState {
public:
  // "Global" configuration state that can actually vary between modules.

  // Ignore all warnings: -w
  unsigned ignoreAllWarnings : 1;

  // Enable all warnings.
  unsigned enableAllWarnings : 1;

  // Treat warnings like errors.
  unsigned warningsAsErrors : 1;

  // Treat errors like fatal errors.
  unsigned errorsAsFatal : 1;

  // Suppress warnings in system headers.
  unsigned suppressSystemWarnings : 1;

  // Map extensions to warnings or errors?
  diag::Level extBehavior = diag::Level::Ignore;

  DiagnosticState()
      : ignoreAllWarnings(false), enableAllWarnings(false),
        warningsAsErrors(false), errorsAsFatal(false),
        suppressSystemWarnings(false) {}

private:
  // Make the state movable only
  DiagnosticState(const DiagnosticState &) = delete;
  const DiagnosticState &operator=(const DiagnosticState &) = delete;

  DiagnosticState(DiagnosticState &&) = default;
  DiagnosticState &operator=(DiagnosticState &&) = default;
};

class InFlightDiagnostic final {
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
class DiagnosticEngine final : public DiagnosticEngineBase, public Printable {
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

  SrcMgr *sm;

  /// The initial diagnostic state.
  mutable DiagnosticState state;

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

public:
  explicit DiagnosticEngine(DiagnosticOptions &diagOpts, SrcMgr *sm = nullptr);

  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;

  void Finish();

public:
  bool HasSrcMgr() const { return sm != nullptr; }
  void SetSrgMgr(SrcMgr *sm) { this->sm = sm; }
  SrcMgr &GetSrcMgr() const {
    assert(sm && "SourceManager not set!");
    return *sm;
  }

  bool HasError();

  void Print(ColorOutputStream &os,
             const PrintingPolicy *policy) const override;

  /// Specify a limit for the number of errors we should
  /// emit before giving up.
  ///
  /// Zero disables the limit.
  void SetErrorLimit(unsigned limit) { diagOpts.errorLimit = limit; }

  /// When set to true, any unmapped warnings are ignored.
  ///
  /// If this and WarningsAsErrors are both set, then this one wins.
  void SetIgnoreAllWarnings(bool status) {
    GetDiagState().ignoreAllWarnings = status;
  }
  bool GetIgnoreAllWarnings() const { return GetDiagState().ignoreAllWarnings; }

  /// Get the actual string in the ".def" for the diagnostic
  llvm::StringRef GetDiagString(const DiagID diagID, bool printDiagnosticName);

  /// Grab the most-recently-added state point.
  DiagnosticState &GetDiagState() const { return state; }

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
  /// Generate EmissionDiagnostic for a Diagnostic to be passed to listeners.
  llvm::Optional<EmissionDiagnostic>
  BuildEmissionDiagnostic(const Diagnostic &diagnostic);

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
    curDiagnostic->GetContext().SetLoc(loc);
    return InFlightDiagnostic(*this, tokenable);
  }

public:
  InFlightDiagnostic Printd(SrcLoc loc, const Diagnostic &diagnostic,
                            Tokenable *tokenable = nullptr) {
    return CreateInFlightDiagnostic(loc, diagnostic, tokenable);
  }

  InFlightDiagnostic Printd(SrcLoc loc, DiagID diagID,
                            llvm::ArrayRef<diag::Argument> args,
                            Tokenable *tokenable = nullptr) {
    return Printd(loc, Diagnostic(DiagnosticContext(diagID, args)), tokenable);
  }

  InFlightDiagnostic Printd(SrcLoc loc, DiagID diagID,
                            Tokenable *tokenable = nullptr) {
    return Printd(
        loc,
        Diagnostic(DiagnosticContext(diagID, llvm::ArrayRef<diag::Argument>())),
        tokenable);
  }
  InFlightDiagnostic Printd(DiagID diagID, Tokenable *tokenable = nullptr) {
    return Printd(
        SrcLoc(),
        Diagnostic(DiagnosticContext(diagID, llvm::ArrayRef<diag::Argument>())),
        tokenable);
  }

  template <typename... ArgTypes>
  InFlightDiagnostic
  Printd(SrcLoc loc, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return Printd(loc, Diagnostic(DiagnosticContext(id, std::move(args)...)));
  }

  template <typename... ArgTypes>
  InFlightDiagnostic
  Printd(SrcLoc loc, Tokenable *tokenable, Diag<ArgTypes...> id,
         typename detail::PassArgument<ArgTypes>::type... args) {
    return Printd(loc, Diagnostic(DiagnosticContext(id, std::move(args)...)),
                  tokenable);
  }
};

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