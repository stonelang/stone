#ifndef STONE_BASIC_DIAGNOSTICENGINE_H
#define STONE_BASIC_DIAGNOSTICENGINE_H

#include "stone/Basic/Diagnostic.h"
#include "stone/Basic/DiagnosticListener.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/List.h"
#include "stone/Basic/Printable.h"

namespace stone {

class SrcMgr;
class DiagnosticEngine;
class InFlightDiagnostic;
class DiagnosticListener;
class LangOptions;
class StoredDiagnostic;

struct DiagnosticStorage {
  /// The maximum number of arguments we can hold. We
  /// currently only support up to 10 arguments (%0-%9).
  ///
  /// A single diagnostic with more than that almost certainly has to
  /// be simplified anyway.
  unsigned maxArguments = 10;

  /// The number of entries in Arguments.
  unsigned char numDiagArgs = 0;

  /// Specifies for each argument whether it is in DiagArgumentsStr
  /// or in DiagArguments.
  // unsigned char DiagArgumentsKind[MaxArguments];

  /// The values for the various substitution positions.
  ///
  /// This is used when the argument is not an std::string. The specific value
  /// is mangled into an intptr_t and the interpretation depends on exactly
  /// what sort of argument kind it is.
  // intptr_t DiagArgumentsVal[MaxArguments];

  /// The values for the various substitution positions that have
  /// string arguments.
  // std::string DiagArgumentsStr[MaxArguments];

  /// The list of ranges added to this diagnostic.
  llvm::SmallVector<CharSrcRange, 8> ranges;

  /// If valid, provides a hint with some code to insert, remove, or
  /// modify at a particular position.
  llvm::SmallVector<CodeFix, 6> fixes;

  /// The diagnostic arguments
  llvm::SmallVector<diag::Argument *, 2> args;

  void AddRange(CharSrcRange range) { ranges.push_back(range); }

  // Avoid copying the fix-it text more than necessary.
  // void AddFixHint(FixHint &&fix) { hints.push_back(std::move(fix)); }

  void AddChildDiagnostic(Diagnostic &&D);

  DiagnosticStorage() = default;
};

class DiagnosticMapping {
public:
};

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
};

class DiagnosticStateMap {
public:
private:
  /// Represents a point in source where the diagnostic state was
  /// modified because of a pragma.
  ///
  /// 'Loc' can be null if the point represents the diagnostic state
  /// modifications done through the command-line.
  struct DiagnosticStatePoint {
    DiagnosticState *state;
    unsigned offset;
    DiagnosticStatePoint(DiagnosticState *state, unsigned offset)
        : state(state), offset(offset) {}
  };
};

struct FlagValueInfo {
  llvm::StringRef data;
  explicit FlagValueInfo(StringRef data) : data(data) {}
};
class InFlightDiagnostic final {
  friend class CodeFixer;
  friend class DiagnosticEngine;
  friend class PartialDiagnostic;

  CodeFixer fixer;
  DiagnosticEngine *de = nullptr;

  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj == 0)),
  // but LLVM is not currently smart enough to eliminate the null check that
  // Emit() would end up with if we used that as our status variable.
  mutable bool isActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool isForceEmit = false;

  InFlightDiagnostic() = default;
  InFlightDiagnostic(const InFlightDiagnostic &) = delete;
  InFlightDiagnostic &operator=(const InFlightDiagnostic &) = delete;
  InFlightDiagnostic &operator=(InFlightDiagnostic &&) = delete;

public:
  InFlightDiagnostic(DiagnosticEngine *de)
      : de(de), fixer(*this), isActive(true) {

    assert(de && "InFlightDiagnostic requires a valid DiagnosticEngine!");
    Flush();
  }

  /// Transfer an in-flight diagnostic to a new object, which is
  /// typically used when returning in-flight diagnostics.
  InFlightDiagnostic(InFlightDiagnostic &&other)
      : de(other.de), fixer(*this), isActive(other.isActive) {
    other.isActive = false;
  }
  /// Emits the diagnostic.
  ~InFlightDiagnostic() { Emit(); }

public:
  CodeFixer &GetFixer() { return fixer; }
  DiagnosticEngine *GetDiagEngine() { return de; }

protected:
  void FlushCounts() {}

  void Flush();

  /// Clear out the current diagnostic.
  void Clear() const {}

  /// Determine whether this diagnostic is still active.
  bool IsActive() const { return isActive; }

  bool Emit() {
    // If this diagnostic is inactive, then its soul was stolen by the copy ctor
    // (or by a subclass, as in SemaInFlightDiagnostic).
    if (!IsActive())
      return false;

    // When emitting diagnostics, we set the final argument count into
    // the DiagnosticEngine object.
    FlushCounts();
    // Process the diagnostic.
    // bool result = de->EmitCurrentDiagnostic(IsForceEmit);

    // This diagnostic is dead.
    Clear();
    // return Result;
    return false;
  }
};

/// Concrete class used by the front-end to report problems and issues.
///
/// This massages the diagnostics (e.g. handling things like "report warnings
/// as errors" and passes them off to the DiagnosticConsumer for reporting to
/// the user. Diagnostics is tied to one translation unit and one
/// SrcMgr.
class DiagnosticEngine final : public Printable {

  friend class InFlightDiagnostic;
  friend class DiagnosticErrorTrap;
  friend class PartialDiagnostic;
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

  // Which overload candidates to show.
  // OverloadsShown ShowOverloads = Ovl_All;

  // Cap on depth of template backtrace stack, 0 -> no limit.
  unsigned templateBacktraceLimit = 0;

  /// The initial diagnostic state.
  DiagnosticState *firstDiagnosticState;

  /// The current diagnostic state.
  DiagnosticState *curDiagnosticState;

  /// The location at which the current diagnostic state was established.
  SrcLoc curDiagnosticStateLoc;

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

  /// Counts for DiagnosticErrorTrap to check whether an error occurred
  /// during a parsing section, e.g. during parsing a function.
  unsigned trapNumErrorsOccurred;
  unsigned trapNumUnrecoverableErrorsOccurred;

  /// The level of the last diagnostic emitted.
  ///
  /// This is used to emit continuation diagnostics with the same level as the
  /// diagnostic that they follow.
  diag::Level lastLevel;

  /// Number of warnings reported
  unsigned numWarnings;

  /// Number of errors reported
  unsigned numErrors;

  LangVersion version;

public:
  explicit DiagnosticEngine(DiagnosticOptions &diagOpts, SrcMgr *sm = nullptr);

  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;
  ~DiagnosticEngine();

public:
  bool HasError();

  void Print(ColorOutputStream &os,
             const PrintingPolicy *policy) const override;

private:
  /// Grab the most-recently-added state point.
  DiagnosticState *GetCurDiagnosticState() const { return curDiagnosticState; }

public:
  bool HasSrcMgr() const { return sm != nullptr; }

  void SetSrgMgr(SrcMgr *sm) { this->sm = sm; }
  SrcMgr &GetSrcMgr() const {
    assert(sm && "SourceManager not set!");
    return *sm;
  }

  Diagnostic &GetCurrentDiagnostic() { return *curDiagnostic; }

  /// Flush the active diagnostic.
  void FlushCurrentDiagnostic();

  void FlushListeners() {
    for (auto listener : listeners)
      listener->Flush();
  }
  /// Copies the current DiagMappings and pushes the new copy
  /// onto the top of the stack.
  void PushMappings(SrcLoc loc);

  /// Pops the current DiagMappings off the top of the stack,
  /// causing the new top of the stack to be the active mappings.
  ///
  /// \returns \c true if the pop happens, \c false if there is only one
  /// DiagMapping on the stack.
  bool PopMappings(SrcLoc loc);

  /// Specify a limit for the number of errors we should
  /// emit before giving up.
  ///
  /// Zero disables the limit.
  void SetErrorLimit(unsigned limit) { diagOpts.errorLimit = limit; }

  /// When set to true, any unmapped warnings are ignored.
  ///
  /// If this and WarningsAsErrors are both set, then this one wins.
  void SetIgnoreAllWarnings(bool status) {
    GetCurDiagnosticState()->ignoreAllWarnings = status;
  }
  // bool GetIgnoreAllWarnings() const {
  //  return GetCurDiagState()->IgnoreAllWarnings;
  // }
  /// Emit the current diagnostic and clear the diagnostic state.
  ///
  /// \param Force Emit the diagnostic regardless of suppression settings.
  bool EmitCurrentDiagnostic(bool force = false);

  /// Get the actual string in the ".def" for the diagnostic
  llvm::StringRef GetDiagString(const DiagID diagID, bool printDiagnosticName);

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

public:
  /// Generate EmissionDiagnostic for a Diagnostic to be passed to listeners.
  llvm::Optional<EmissionDiagnostic>
  GetEmissionDiagnosticForDiagnostic(const Diagnostic &diagnostic);

  // Send \c diag to all diagnostic consumers.
  void EmitDiagnostic(const Diagnostic &diag);

  /// Send all tentative diagnostics to all diagnostic consumers and
  /// delete them.
  void EmitPendingDiagnostics();

public:
  InFlightDiagnostic Diagnose(SrcLoc loc, const Diagnostic &diagnostic) {
    assert(!curDiagnostic && "Already have an active diagnostic");
    curDiagnostic = diagnostic;
    curDiagnostic->GetContext().SetLoc(loc);
    return InFlightDiagnostic(this);
  }

  InFlightDiagnostic Diagnose(SrcLoc loc, DiagID diagID,
                              llvm::ArrayRef<diag::Argument> args) {
    return Diagnose(loc, Diagnostic(DiagnosticContext(diagID, args)));
  }

  InFlightDiagnostic Diagnose(SrcLoc loc, DiagID diagID) {
    return Diagnose(loc, Diagnostic(DiagnosticContext(
                             diagID, llvm::ArrayRef<diag::Argument>())));
  }

  template <typename... ArgTypes>
  InFlightDiagnostic
  Diagnose(SrcLoc loc, Diag<ArgTypes...> id,
           typename detail::PassArgument<ArgTypes>::type... args) {

    return Diagnose(loc, Diagnostic(DiagnosticContext(id, std::move(args)...)));
  }

  /// Determine whethere there is already a diagnostic in flight -- there is a
  /// better way.
  bool HasInflightDiagnostic() {
    return (unsigned)curDiagnostic->GetContext().GetDiagID() !=
           std::numeric_limits<unsigned>::max();
  }
};

// NOTE: This may be the better way to do this.
inline InFlightDiagnostic &operator<<(InFlightDiagnostic &inFlightDiagnostic,
                                      diag::Argument &&argument) {
  inFlightDiagnostic.GetDiagEngine()
      ->GetCurrentDiagnostic()
      .GetContext()
      .AddArgument(std::move(argument));

  return inFlightDiagnostic;
}
// inline InFlightDiagnostic &operator<<(InFlightDiagnostic &inFlightDiagnostic,
//                                       CodeFix &&fix) {
//   inFlightDiagnostic.GetDiagEngine()
//       ->GetCurrentDiagnostic()
//       .GetContext()
//       .AddFix(std::move(fix));

//   return inFlightDiagnostic;
// }

/// RAII class that determines when any errors have occurred
/// between the time the instance was created and the time it was
/// queried.
///
/// Note that you almost certainly do not want to use this. It's usually
/// meaningless to ask whether a particular scope triggered an error message,
/// because error messages outside that scope can mark things invalid (or cause
/// us to reach an error limit), which can suppress errors within that scope.
class DiagnosticErrorTrap {
  DiagnosticEngine &de;
  unsigned numErrors;
  unsigned numUnrecoverableErrors;

public:
  explicit DiagnosticErrorTrap(DiagnosticEngine &de) : de(de) { Reset(); }

  /// Determine whether any errors have occurred since this
  /// object instance was created.
  bool HasErrorOccurred() const { return de.trapNumErrorsOccurred > numErrors; }

  /// Determine whether any unrecoverable errors have occurred since this
  /// object instance was created.
  bool HasUnrecoverableErrorOccurred() const {
    return de.trapNumUnrecoverableErrorsOccurred > numUnrecoverableErrors;
  }

  /// Set to initial state of "no errors occurred".
  void Reset() {
    numErrors = de.trapNumErrorsOccurred;
    numUnrecoverableErrors = de.trapNumUnrecoverableErrorsOccurred;
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
