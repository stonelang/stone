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
class LiveDiagnostic;
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
  llvm::SmallVector<DiagnosticArgument *, 2> args;

  void AddRange(CharSrcRange range) { ranges.push_back(range); }

  // Avoid copying the fix-it text more than necessary.
  //void AddFixHint(FixHint &&fix) { hints.push_back(std::move(fix)); }

  void AddChildDiagnostic(Diagnostic &&D);

  DiagnosticStorage() = default;
};

class DiagnosticMapping {
public:
};

class DiagnosticState {
  llvm::DenseMap<unsigned, DiagnosticMapping> diagMapping;

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

  using iterator = llvm::DenseMap<unsigned, DiagnosticMapping>::iterator;
  using const_iterator =
      llvm::DenseMap<unsigned, DiagnosticMapping>::const_iterator;

  /*
      void SetMapping(diag::kind Diag, DiagnosticMapping Info) {
        DiagMap[Diag] = Info;
      }

      DiagnosticMapping PickMapping(diag::kind Diag) const {
        return DiagMap.lookup(Diag);
      }

      DiagnosticMapping &GetOrAddMapping(diag::kind Diag);

      const_iterator begin() const { return DiagMap.begin(); }
      const_iterator end() const { return DiagMap.end(); }

  */
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

/// Concrete class used by the front-end to report problems and issues.
///
/// This massages the diagnostics (e.g. handling things like "report warnings
/// as errors" and passes them off to the DiagnosticConsumer for reporting to
/// the user. Diagnostics is tied to one translation unit and one
/// SrcMgr.
class DiagnosticEngine final : public llvm::RefCountedBase<DiagnosticEngine>,
                               public Printable {

  friend class LiveDiagnostic;
  friend class DiagnosticErrorTrap;
  friend class PartialDiagnostic;
  friend struct DiagnosticArgument;

  /// The
  unsigned int diagnosticSeen = 0;

  /// The diagnostic listeners(s) that will be responsible for actually
  /// emitting diagnostics.
  llvm::SmallVector<DiagnosticListener *, 2> listeners;

  /// The currently diagnostic, if there is one.
  llvm::Optional<Diagnostic> curDiagnostic;

  // llvm::IntrusiveRefCntPtr<DiagnosticOptions> diagOptions;

  DiagnosticOptions &diagOpts;

  SrcMgr *sm;

private:
  // Which overload candidates to show.
  // OverloadsShown ShowOverloads = Ovl_All;

  // Cap on depth of template backtrace stack, 0 -> no limit.
  // unsigned TemplateBacktraceLimit = 0;

  // Cap on depth of constexpr evaluation backtrace stack, 0 -> no limit.
  // unsigned ConstexprBacktraceLimit = 0;
  //
  //
  //
  /// The initial diagnostic state.
  DiagnosticState *firstDiagnosticState;

  /// The current diagnostic state.
  DiagnosticState *curDiagnosticState;

  /// The location at which the current diagnostic state was established.
  SrcLoc curDiagnosticStateLoc;

  /// Optional flag value.
  ///
  /// Some flags accept values, for instance: -Wframe-larger-than=<value> and
  /// -Rpass=<value>. The content of this string is emitted after the flag name
  /// and '='.
  std::string flagValue;

  /// ID of the "delayed" diagnostic, which is a (typically
  /// fatal) diagnostic that had to be delayed because it was found
  /// while emitting another diagnostic.
  unsigned delayedDiagID;

  //  SUCKS
  struct DelayedDiagArgument {
    /// First string argument for the delayed diagnostic.
    std::string one;

    /// Second string argument for the delayed diagnostic.
    std::string two;

    /// Third string argument for the delayed diagnostic.
    std::string three;
  };

  DelayedDiagArgument delayedDiagArgument;

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
  /// Issue the message to the client.
  ///
  /// This actually returns an instance of LiveDiagnostic which emits the
  /// diagnostics (through @c ProcessDiag) when it is destroyed.
  ///
  /// \param DiagID A member of the @c diag::kind enum.
  /// \param Loc Represents the source location associated with the diagnostic,
  /// which can be an invalid location if no position information is available.
  // inline LiveDiagnostic Issue(SrcLoc loc, unsigned diagID);
  // inline LiveDiagnostic Issue(unsigned DiagID);
  // void Issue(const StoredDiagnostic &storedDiagnostic);

  // inline LiveDiagnostic Issue(SrcLoc loc, DiagID diagID);

  inline LiveDiagnostic Diagnose(SrcLoc loc, DiagID diagID,
                                 llvm::ArrayRef<DiagnosticArgument> args);

  inline LiveDiagnostic Diagnose(SrcLoc loc, const Diagnostic &diagnostic);

  template <typename... ArgTypes>
  inline LiveDiagnostic
  Diagnose(SrcLoc loc, Diag<ArgTypes...> id,
           typename detail::PassArgument<ArgTypes>::type... args);

  template <typename... ArgTypes>
  inline LiveDiagnostic
  Diagnose(Diag<ArgTypes...> id,
           typename detail::PassArgument<ArgTypes>::type... args);

  /// Determine whethere there is already a diagnostic in flight -- there is a
  /// better way.
  bool IsInflight() {
    return (unsigned)curDiagnostic->GetDiagContext().GetDiagID() !=
           std::numeric_limits<unsigned>::max();
  }
};

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

class StreamingDiagnostic {
protected:
  mutable DiagnosticEngine *de = nullptr;

public:
  StreamingDiagnostic(DiagnosticEngine *de) : de(de) {}

public:
  void AddArgument(const DiagnosticArgument &arg) {}


  void AddRange(const CharSrcRange &range) const {
    //assert(!de && "Null DiagnosticEngine");
  }

  void AddFix(const CodeFix &fix) const {
    // if (Hint.IsNull())
    //   return;
    // de->GetCurrentDiagnostic().GetDiagContext().hints.push_back(hint);
  }
};

class LiveDiagnostic final : public StreamingDiagnostic {
  friend class DiagnosticEngine;
  // friend class PartialDiagnostic;

  mutable unsigned numArgs = 0;

  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj == 0)),
  // but LLVM is not currently smart enough to eliminate the null check that
  // Emit() would end up with if we used that as our status variable.
  mutable bool isActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool isForceEmit = false;

  LiveDiagnostic() = default;

  explicit LiveDiagnostic(DiagnosticEngine *de)
      : StreamingDiagnostic(de), isActive(true) {

    assert(de && "LiveDiagnostic requires a valid DiagnosticEngine!");
    de->GetCurrentDiagnostic().GetDiagContext().Flush();
  }

public:
  /// Issue the message to the client.
  ///
  /// This actually returns an instance of LiveDiagnostic which emits the
  /// diagnostics (through @c ProcessDiag) when it is destroyed.
  ///
  /// \param DiagID A member of the @c diag::kind enum.
  /// \param Loc Represents the source location associated with the diagnostic,
  /// which can be an invalid location if no position information is available.
  // inline LiveDiagnostic Emit(const SrcLoc loc, const unsigned diagnosticID,
  //                                const unsigned msgID);

  // inline LiveDiagnostic Emit(const unsigned diagnosticID, const unsigned
  // msgID);

  // TODO: Remove
  void AddFlagValue(llvm::StringRef data) const {
    de->flagValue = std::string(data);
  }

protected:
  void FlushCounts() {}

  /// Clear out the current diagnostic.
  void Clear() const {}

  /// Determine whether this diagnostic is still active.
  bool IsActive() const { return isActive; }

  bool Emit() {
    // If this diagnostic is inactive, then its soul was stolen by the copy ctor
    // (or by a subclass, as in SemaLiveDiagnostic).
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

public:
  LiveDiagnostic &operator=(const LiveDiagnostic &) = delete;

  /// Emits the diagnostic.
  ~LiveDiagnostic() { Emit(); }

public:
  template <typename T> const LiveDiagnostic &operator<<(const T &v) const {
    assert(IsActive() && "Clients must not add to cleared diagnostic!");
    const StreamingDiagnostic &sd = *this;
    sd << v;
    return *this;
  }
};

// inline LiveDiagnostic DiagnosticEngine::Issue(SrcLoc loc, unsigned diagID) {
//   assert(curDiagID == std::numeric_limits<unsigned>::max() &&
//          "Multiple diagnostics in flight at once!");

//   // CurDiagLoc = Loc;
//   // CurDiagID = DiagID;
//   // FlagValue.clear();
//   return LiveDiagnostic(this);
// }
// const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
//                                     llvm::Error &&E);

// inline LiveDiagnostic DiagnosticEngine::Issue(unsigned diagID) {
//   return Issue(SrcLoc(), diagID);
// }

/*
/// Register a value for the flag in the current diagnostic. This
/// value will be shown as the suffix "=value" after the flag name. It is
/// useful in cases where the diagnostic flag accepts values (e.g.,
/// -Rpass or -Wframe-larger-than).
inline const LiveDiagnostic &operator<<(const LiveDiagnostic &live,
                                           const AddFlagValue V) {
  live.AddFlagValue(V.Val);
  return live;
}
*/

inline const LiveDiagnostic &operator<<(const LiveDiagnostic &live,
                                        llvm::StringRef data) {
  // live.AddString(data);
  return live;
}

inline const LiveDiagnostic &operator<<(const LiveDiagnostic &live,
                                        const char *data) {

  // live.AddTaggedVal(reinterpret_cast<intptr_t>(data),
  //                DiagnosticArgumentType::CStr);
  return live;
}

// inline const LiveDiagnostic &operator<<(const LiveDiagnostic &live,
// int data) { live.AddTaggedVal(data, DiagnosticArgumentType::SInt); return
// live;
// }

class StoredDiagnostic {
  // unsigned diagIdentifier;
  // diag::Level level;
  // FullSourceLoc loc;
  // std::string message;
  // std::vector<CharSrcRange> ranges;
  // std::vector<FixHint> hints;

public:
  StoredDiagnostic() = default;
};

inline LiveDiagnostic DiagnosticEngine::Diagnose(SrcLoc loc,
                                                 const Diagnostic &diagnostic) {
  assert(!curDiagnostic && "Already have an active diagnostic");
  curDiagnostic = diagnostic;
  curDiagnostic->SetLoc(loc);
  return LiveDiagnostic(this);
}

inline LiveDiagnostic
DiagnosticEngine::Diagnose(SrcLoc loc, DiagID diagID,
                           llvm::ArrayRef<DiagnosticArgument> args) {
  return Diagnose(loc, Diagnostic(DiagnosticContext(diagID, args)));
}

template <typename... ArgTypes>
inline LiveDiagnostic DiagnosticEngine::Diagnose(
    SrcLoc loc, Diag<ArgTypes...> id,
    typename detail::PassArgument<ArgTypes>::type... args) {
  return LiveDiagnostic(this);
}

template <typename... ArgTypes>
inline LiveDiagnostic DiagnosticEngine::Diagnose(
    Diag<ArgTypes...> id,
    typename detail::PassArgument<ArgTypes>::type... args) {
  return Diagnose(SrcLoc(), id, std::forward<ArgTypes>(args)...);
}

// inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &live,
//                                              llvm::StringRef S) {
//   live.AddString(S);
//   return live;
// }

inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &live,
                                             llvm::ArrayRef<CodeFix> fixes) {
  for (const CodeFix &fix : fixes) {
    // live.AddHint(hint);
  }
  return live;
}

inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &live,
                                             const DiagnosticArgument &arg) {

  return live;
}

} // namespace stone
#endif
