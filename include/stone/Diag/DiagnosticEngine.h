#ifndef STONE_DIAG_DIAGNOSTIC_ENGINE_H
#define STONE_DIAG_DIAGNOSTIC_ENGINE_H

#include "stone/AST/Identifier.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticID.h"
#include "stone/Support/DiagnosticOptions.h"

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

struct DiagnosticStorage final {};

enum class DiagnosticArgumentKind {
  None = 0,
  Integer,
  Unsigned,
  String,
  Identifier,
};
class DiagnosticArgument {
  DiagnosticArgumentKind Kind;
  union {
    int IntegerVal;
    unsigned UnsignedVal;
    llvm::StringRef StringVal;
    Identifier IdentifierVal;
  };

public:
  DiagnosticArgument(llvm::StringRef S)
      : Kind(DiagnosticArgumentKind::String), StringVal(S) {}

  DiagnosticArgument(int I)
      : Kind(DiagnosticArgumentKind::Integer), IntegerVal(I) {}

  DiagnosticArgument(unsigned I)
      : Kind(DiagnosticArgumentKind::Unsigned), UnsignedVal(I) {}

  DiagnosticArgument(Identifier I)
      : Kind(DiagnosticArgumentKind::Identifier), IdentifierVal(I) {}

  /// Initializes a diagnostic argument using the underlying type of the
  /// given enum.
  template <
      typename EnumType,
      typename std::enable_if<std::is_enum<EnumType>::value>::type * = nullptr>
  DiagnosticArgument(EnumType value)
      : DiagnosticArgument(
            static_cast<typename std::underlying_type<EnumType>::type>(value)) {
  }

public:
  llvm::StringRef GetAsString() const {
    assert(Kind == DiagnosticArgumentKind::String);
    return StringVal;
  }

  int GetAsInteger() const {
    assert(Kind == DiagnosticArgumentKind::Integer);
    return IntegerVal;
  }

  unsigned GetAsUnsigned() const {
    assert(Kind == DiagnosticArgumentKind::Unsigned);
    return UnsignedVal;
  }

  Identifier GetAsIdentifier() const {
    assert(Kind == DiagnosticArgumentKind::Identifier);
    return IdentifierVal;
  }
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

  DiagnosticOptions &DiagOpts;

  SrcMgr &SM;

  /// The diagnostic consumer(s) that will be responsible for actually
  /// emitting diagnostics.
  llvm::SmallVector<DiagnosticClient *, 2> Clients;

  // DiagIDContext diagIDContext;

  /// The ID of the current diagnostic that is in flight.
  ///
  /// This is set to std::numeric_limits<unsigned>::max() when there is no
  /// diagnostic in flight.
  DiagID CurDiagID;
  SrcLoc CurDiagLoc;

private:
  class DiagnosticState {
  public:
  };

  class DiagnosticStateMap {
  public:
  };

  struct DiagnosticStatePoint {};

  struct File {};

public:
  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;

  explicit DiagnosticEngine(DiagnosticOptions &DiagOpts, SrcMgr &SM);
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
  InFlightDiagnostic Diagnose(SrcLoc NextDiagLoc, DiagID NextDiagID);

  /// Determine whethere there is already a diagnostic in flight.
  bool IsInFlightDiagnostic() const {
    return CurDiagID != std::numeric_limits<DiagID>::max();
  }

  DiagID GetCurDiagID() const { return CurDiagID; }
  SrcLoc GetDiagLoc() const { return CurDiagLoc; }
  /// Get from diag options bool ShouldShowColors() { return }

public:
  void FinishProcessing();

public:
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

public:
  // InFlightDiagnostic &FixItReplace(SrcRange R, StringRef Str);
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