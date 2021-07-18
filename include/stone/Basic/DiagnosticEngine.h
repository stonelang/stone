#ifndef STONE_BASIC_DIAGNOSTICENGINE_H
#define STONE_BASIC_DIAGNOSTICENGINE_H

#include "stone/Basic/DiagnosticArgument.h"
#include "stone/Basic/DiagnosticListener.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/List.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Error.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace stone {

class SrcMgr;
class DiagnosticEngine;
class LiveDiagnostic;
class DiagnosticListener;
class LangOptions;
class StoredDiagnostic;

/// Enumeration describing all of possible diagnostics.
///
/// Each of the diagnostics described in DiagnosticEngine.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : uint32_t;

enum class FixID : uint32_t;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... argTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID diagID;
};

namespace detail {
/// Describes how to pass a diagnostic argument of the given type.
///
/// By default, diagnostic arguments are passed by value, because they
/// tend to be small. Larger diagnostic arguments
/// need to specialize this class template to pass by reference.
template <typename T> struct PassArgument { typedef T type; };
} // namespace detail

class FixHint final {
public:
  /// Code that should be replaced to correct the error. Empty for an
  /// insertion hint.
  CharSrcRange removeRange;

  /// Code in the specific range that should be inserted in the insertion
  /// location.
  CharSrcRange insertFromRange;

  /// The actual code to insert at the insertion location, as a
  /// string.
  std::string codeToInsert;

  bool beforePreviousInsertions = false;

  /// Empty code modification hint, indicating that no code
  /// modification is known.
  FixHint() = default;

  bool IsNull() const { return !removeRange.isValid(); }

  /// Create a code modification hint that inserts the given
  /// code string at a specific location.
  static FixHint CreateInsertion(SrcLoc insertionLoc, StringRef code,
                                 bool beforePreviousInsertions = false) {
    FixHint fix;
    fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
    fix.codeToInsert = std::string(code);
    fix.beforePreviousInsertions = beforePreviousInsertions;
    return fix;
  }

  /// Create a code modification hint that inserts the given
  /// code from \p FromRange at a specific location.
  static FixHint
  CreateInsertionFromRange(SrcLoc insertionLoc, CharSrcRange fromRange,
                           bool beforePreviousInsertions = false) {
    FixHint fix;
    fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
    fix.insertFromRange = fromRange;
    fix.beforePreviousInsertions = beforePreviousInsertions;
    return fix;
  }

  /// Create a code modification hint that removes the given
  /// source range.
  static FixHint CreateRemoval(CharSrcRange removeRange) {
    FixHint fix;
    fix.removeRange = removeRange;
    return fix;
  }
  static FixHint CreateRemoval(SrcRange removeRange) {
    return CreateRemoval(CharSrcRange::getTokenRange(removeRange));
  }

  /// Create a code modification hint that replaces the given
  /// source range with the given code string.
  static FixHint CreateReplacement(CharSrcRange removeRange,
                                   llvm::StringRef code) {
    FixHint fix;
    fix.removeRange = removeRange;
    fix.codeToInsert = std::string(code);
    return fix;
  }

  static FixHint CreateReplacement(SrcRange removeRange, llvm::StringRef code) {
    return CreateReplacement(CharSrcRange::getTokenRange(removeRange), code);
  }
};
struct DiagnosticFormatOptions final {};
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
  llvm::SmallVector<FixHint, 6> hints;

  /// The diagnostic arguments
  llvm::SmallVector<DiagnosticArgument *, 2> args;

  void AddRange(CharSrcRange range) { ranges.push_back(range); }

  // Avoid copying the fix-it text more than necessary.
  void AddFixHint(FixHint &&fix) { hints.push_back(std::move(fix)); }

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
class DiagnosticEngine final : public llvm::RefCountedBase<DiagnosticEngine> {

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
  // llvm::Optional<Diagnostic> curDiagnostic;

  // TODO: Remove
  const DiagnosticOptions &diagOpts;

  // llvm::IntrusiveRefCntPtr<DiagnosticOptions> diagOptions;

  SrcMgr *sm = nullptr;

private:
  /// The location of the current diagnostic that is in flight.
  SrcLoc curDiagLoc;
  /// The ID of the current diagnostic that is in flight.
  ///
  /// This is set to std::numeric_limits<unsigned>::max() when there is no
  /// diagnostic in flight.
  unsigned curDiagID;

  // TODO: DiagID curDiagID;
private:
  // Treat fatal errors like errors.
  bool fatalsAsError = false;

  // Suppress all diagnostics.
  bool suppressAllDiagnostics = false;

  // Elide common types of templates.
  bool elideType = true;

  // Print a tree when comparing templates.
  bool printTemplateTree = false;

  // Color printing is enabled.
  bool showColors = false;

  // Which overload candidates to show.
  // OverloadsShown ShowOverloads = Ovl_All;

  // Cap of # errors emitted, 0 -> no limit.
  unsigned errorLimit = 0;

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
  explicit DiagnosticEngine(const DiagnosticOptions &diagOpts,
                            SrcMgr *sm = nullptr);

  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;
  ~DiagnosticEngine();

public:
  bool HasError();
  void Print();

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

  /// Copies the current DiagMappings and pushes the new copy
  /// onto the top of the stack.
  void PushMappings(SrcLoc loc);

  /// Pops the current DiagMappings off the top of the stack,
  /// causing the new top of the stack to be the active mappings.
  ///
  /// \returns \c true if the pop happens, \c false if there is only one
  /// DiagMapping on the stack.
  bool PopMappings(SrcLoc loc);

  void SetCurrentListener(DiagnosticListener *listener);

  // void AddArgument(DiagnosticArgument *argument);

  /// Specify a limit for the number of errors we should
  /// emit before giving up.
  ///
  /// Zero disables the limit.
  void SetErrorLimit(unsigned limit) { errorLimit = limit; }

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

  unsigned GetCurrentDiagID() const { return curDiagID; }

  SrcLoc GetCurrentDiagLoc() const { return curDiagLoc; }

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

  // inline LiveDiagnostic Issue(const Diagnosable *custom, DiagID diagID,
  //                              llvm::ArrayRef<DiagnosticArgument> args);

  /// Determine whethere there is already a diagnostic in flight.
  bool IsLive() const {
    return curDiagID != std::numeric_limits<unsigned>::max();
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
public:
};

class LiveDiagnostic final : public StreamingDiagnostic {
  friend class DiagnosticEngine;
  // friend class PartialDiagnostic;

  mutable DiagnosticEngine *de = nullptr;
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

  explicit LiveDiagnostic(DiagnosticEngine *de) : de(de), isActive(true) {
    assert(de && "LiveDiagnostic requires a valid DiagnosticEngine!");

    // de->ranges.clear();
    // de->fixHints.clear();
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
  /// Copy constructor.  When copied, this "takes" the diagnostic info from the
  /// input and neuters it.
  LiveDiagnostic(const LiveDiagnostic &live) {}
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

/// Pass the D
class DiagnosticContext final {

  DiagID diagID;
  DiagnosticEngine *de;
  llvm::SmallVector<DiagnosticArgument, 3> args;
  llvm::SmallVector<CharSrcRange, 2> ranges;
  llvm::SmallVector<FixHint, 2> hints;
public:
  template <typename... ArgTypes>
  DiagnosticContext(Diag<ArgTypes...> d,
                    typename detail::PassArgument<ArgTypes>::type... vArgs)
      : diagID(d.diagID) {

    auto diagArgs = {std::move(vArgs)...};
    args.append(diagArgs + 1, diagArgs + 1 + sizeof...(vArgs));
  }
public:
  DiagnosticContext(DiagID diagID, llvm::ArrayRef<DiagnosticArgument> arguments)
      : diagID(diagID), args(arguments.begin(), arguments.end()) {}

public:
  DiagID GetDiagID() { return diagID; }
  llvm::ArrayRef<DiagnosticArgument> GetArgs() const { return args; }
  llvm::ArrayRef<CharSrcRange> GetRanges() const { return ranges; }
  llvm::ArrayRef<FixHint> GetFixHints() const { return hints; }
};
class Diagnostic {
  SrcLoc loc;
  DiagnosticContext diagContext;

public:
  explicit Diagnostic(DiagnosticContext diagContext)
      : diagContext(diagContext) {}
public:
  void SetLoc(SrcLoc sl) { loc = sl; }
  SrcLoc GetLoc() { return loc; }

  DiagnosticContext &GetDiagContext() { return diagContext; }

  // CStrDiagnosticArgument GetCStrDiagnosticArgument() {}

  // const DiagnosticsEngine *getDiags() const { return DiagObj; }
  // unsigned getID() const { return DiagObj->CurDiagID; }
  // const SourceLocation &getLocation() const { return DiagObj->CurDiagLoc; }
  // bool hasSourceManager() const { return DiagObj->hasSourceManager(); }
  // SourceManager &getSourceManager() const { return
  // DiagObj->getSourceManager();}

  // unsigned getNumArgs() const { return DiagObj->DiagStorage.NumDiagArgs; }

  // /// Return the kind of the specified index.
  // ///
  // /// Based on the kind of argument, the accessors below can be used to get
  // /// the value.
  // ///
  // /// \pre Idx < getNumArgs()
  // DiagnosticsEngine::ArgumentKind getArgKind(unsigned Idx) const {
  //   assert(Idx < getNumArgs() && "Argument index out of range!");
  //   return (DiagnosticsEngine::ArgumentKind)
  //       DiagObj->DiagStorage.DiagArgumentsKind[Idx];
  // }

  // /// Return the provided argument string specified by \p Idx.
  // /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_std_string
  // const std::string &getArgStdStr(unsigned Idx) const {
  //   assert(getArgKind(Idx) == DiagnosticsEngine::ak_std_string &&
  //          "invalid argument accessor!");
  //   return DiagObj->DiagStorage.DiagArgumentsStr[Idx];
  // }

  // /// Return the specified C string argument.
  // /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_c_string
  // const char *getArgCStr(unsigned Idx) const {
  //   assert(getArgKind(Idx) == DiagnosticsEngine::ak_c_string &&
  //          "invalid argument accessor!");
  //   return reinterpret_cast<const char *>(
  //       DiagObj->DiagStorage.DiagArgumentsVal[Idx]);
  // }

  // /// Return the specified signed integer argument.
  // /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_sint
  // int getArgSInt(unsigned Idx) const {
  //   assert(getArgKind(Idx) == DiagnosticsEngine::ak_sint &&
  //          "invalid argument accessor!");
  //   return (int)DiagObj->DiagStorage.DiagArgumentsVal[Idx];
  // }

  // /// Return the specified unsigned integer argument.
  // /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_uint
  // unsigned getArgUInt(unsigned Idx) const {
  //   assert(getArgKind(Idx) == DiagnosticsEngine::ak_uint &&
  //          "invalid argument accessor!");
  //   return (unsigned)DiagObj->DiagStorage.DiagArgumentsVal[Idx];
  // }

  // /// Return the specified IdentifierInfo argument.
  // /// \pre getArgKind(Idx) == DiagnosticsEngine::ak_identifierinfo
  // const IdentifierInfo *getArgIdentifier(unsigned Idx) const {
  //   assert(getArgKind(Idx) == DiagnosticsEngine::ak_identifierinfo &&
  //          "invalid argument accessor!");
  //   return reinterpret_cast<IdentifierInfo *>(
  //       DiagObj->DiagStorage.DiagArgumentsVal[Idx]);
  // }

  // /// Return the specified non-string argument in an opaque form.
  // /// \pre getArgKind(Idx) != DiagnosticsEngine::ak_std_string
  // intptr_t getRawArg(unsigned Idx) const {
  //   assert(getArgKind(Idx) != DiagnosticsEngine::ak_std_string &&
  //          "invalid argument accessor!");
  //   return DiagObj->DiagStorage.DiagArgumentsVal[Idx];
  // }

  // /// Return the number of source ranges associated with this diagnostic.
  // unsigned getNumRanges() const {
  //   return DiagObj->DiagStorage.DiagRanges.size();
  // }

  // /// \pre Idx < getNumRanges()
  // const CharSourceRange &getRange(unsigned Idx) const {
  //   assert(Idx < getNumRanges() && "Invalid diagnostic range index!");
  //   return DiagObj->DiagStorage.DiagRanges[Idx];
  // }

  // /// Return an array reference for this diagnostic's ranges.
  // ArrayRef<CharSourceRange> getRanges() const {
  //   return DiagObj->DiagStorage.DiagRanges;
  // }

  // unsigned getNumFixItHints() const {
  //   return DiagObj->DiagStorage.FixItHints.size();
  // }

  // const FixItHint &getFixItHint(unsigned Idx) const {
  //   assert(Idx < getNumFixItHints() && "Invalid index!");
  //   return DiagObj->DiagStorage.FixItHints[Idx];
  // }

  // ArrayRef<FixItHint> getFixItHints() const {
  //   return DiagObj->DiagStorage.FixItHints;
  // }

  /// Format this diagnostic into a string, substituting the
  /// formal arguments into the %0 slots.
  ///
  /// The result is appended onto the \p OutStr array.
  virtual void Format(llvm::SmallVectorImpl<char> &outStr,
                      const DiagnosticFormatOptions &fmtOptions) const;

  /// Format the given format-string into the output buffer using the
  /// arguments stored in this diagnostic.
  virtual void Format(const char *diagStr, const char *diagEnd,
                      llvm::SmallVectorImpl<char> &outStr,
                      const DiagnosticFormatOptions &fmtOptions) const;
};
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
  // TODO:
  // assert(!curDiagnostic && "Already have an active diagnostic");
  // curDiagnostic = diagnostic;
  // curDiagnostic->SetLoc(Loc);

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

} // namespace stone
#endif
