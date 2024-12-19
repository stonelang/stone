#ifndef STONE_DIAG_DIAGNOSTIC_ENGINE_H
#define STONE_DIAG_DIAGNOSTIC_ENGINE_H

#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/DiagnosticClient.h"
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
class IdentifierInfo;

namespace diags {

class InFlightDiagnostic;
class DiagnosticClient;
class Identifier;
class LangOptions;

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
    IdentifierInfo *IdentifierVal;
  };

public:
  DiagnosticArgument(llvm::StringRef S)
      : Kind(DiagnosticArgumentKind::String), StringVal(S) {}

  DiagnosticArgument(int I)
      : Kind(DiagnosticArgumentKind::Integer), IntegerVal(I) {}

  DiagnosticArgument(unsigned I)
      : Kind(DiagnosticArgumentKind::Unsigned), UnsignedVal(I) {}

  DiagnosticArgument(IdentifierInfo *I)
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

  IdentifierInfo *GetAsIdentifier() const {
    assert(Kind == DiagnosticArgumentKind::Identifier);
    return IdentifierVal;
  }
};

/// DiagnosticRenderer in clang
class DiagnosticEngine final {
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
  unsigned NumOverloadsToShow = 32;

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
  unsigned TrapNumErrorsOccurred;
  unsigned TrapNumUnrecoverableErrorsOccurred;

  /// The level of the last diagnostic emitted.
  ///
  /// This is used to emit continuation diagnostics with the same level as the
  /// diagnostic that they follow.
  DiagnosticLevel LastDiagLevel;

  /// Number of warnings reported
  unsigned NumWarnings;

  /// Number of errors reported
  unsigned NumErrors;

  DiagnosticOptions &DiagOpts;

  SrcMgr &SM;

  DiagnosticClient *Client = nullptr;
  std::unique_ptr<DiagnosticClient> ClientOwner;

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
  void SetClient(DiagnosticClient *client);
  /// Return the current diagnostic client along with ownership of that
  /// client.
  std::unique_ptr<DiagnosticClient> TakeClient();

public:
};

class DiagnosticErrorTrap {
public:
};

class StreamingDiagnostic {
public:
};

class InFlightDiagnostic : public StreamingDiagnostic {
public:
};

class StoredDiagnostic {
public:
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