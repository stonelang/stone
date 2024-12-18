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
namespace diags {

class InFlightDiagnostic;
class DiagnosticClient;
class Identifier;
class LangOptions;

struct DiagnosticStorage final {};

enum class DiagnosticArgumentKind {
  None = 0,

};

/// DiagnosticRenderer in clang
class DiagnosticEngine final {

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
  explicit DiagnosticEngine();

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