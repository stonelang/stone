#include "stone/Syntax/Diagnostics.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTPrinter.h"
#include "stone/Syntax/Decl.h"
//#include "stone/Syntax/DiagnosticSuppression.h"
#include "stone/Syntax/DiagnosticsBasic.h"
//#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Module.h"
//#include "stone/Syntax/Pattern.h"
//#include "stone/Syntax/PrintOptions.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"

#include "stone/Basic/SrcMgr.h"
//#include "stone/Config.h"
// #include "stone/Localization/LocalizationFormat.h"
// #include "stone/Parse/Lexer.h" // bad dependency

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"



using namespace stone;
using namespace stone::diag;

// static_assert(IsTriviallyDestructible<ZeroArgDiagnostic>::value,
//               "ZeroArgDiagnostic is meant to be trivially destructable");

namespace {
enum class DiagnosticOptions {
  /// No options.
  none,

  /// The location of this diagnostic points to the beginning of the first
  /// token that the parser considers invalid.  If this token is located at the
  /// beginning of the line, then the location is adjusted to point to the end
  /// of the previous token.
  ///
  /// This behavior improves experience for "expected token X" diagnostics.
  PointsToFirstBadToken,

  /// After a fatal error subsequent diagnostics are suppressed.
  Fatal,

  /// An API or ABI breakage diagnostic emitted by the API digester.
  APIDigesterBreakage,

  /// A deprecation warning or error.
  Deprecation,

  /// A diagnostic warning about an unused element.
  NoUsage,
};
struct StoredDiagnosticInfo {
  DiagnosticKind kind : 2;
  bool pointsToFirstBadToken : 1;
  bool isFatal : 1;
  bool isAPIDigesterBreakage : 1;
  bool isDeprecation : 1;
  bool isNoUsage : 1;

  constexpr StoredDiagnosticInfo(DiagnosticKind k, bool firstBadToken,
                                 bool fatal, bool isAPIDigesterBreakage,
                                 bool deprecation, bool noUsage)
      : kind(k), pointsToFirstBadToken(firstBadToken), isFatal(fatal),
        isAPIDigesterBreakage(isAPIDigesterBreakage), isDeprecation(deprecation),
        isNoUsage(noUsage) {}
  constexpr StoredDiagnosticInfo(DiagnosticKind k, DiagnosticOptions opts)
      : StoredDiagnosticInfo(k,
                             opts == DiagnosticOptions::PointsToFirstBadToken,
                             opts == DiagnosticOptions::Fatal,
                             opts == DiagnosticOptions::APIDigesterBreakage,
                             opts == DiagnosticOptions::Deprecation,
                             opts == DiagnosticOptions::NoUsage) {}
};

// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum LocalDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/Syntax/Diagnostics.def"
  NumDiags
};

// TODO: categorization
static const constexpr StoredDiagnosticInfo storedDiagnosticInfos[] = {
#define ERROR(ID, Options, Text, Signature)                                    \
  StoredDiagnosticInfo(DiagnosticKind::Error, DiagnosticOptions::Options),
#define WARNING(ID, Options, Text, Signature)                                  \
  StoredDiagnosticInfo(DiagnosticKind::Warning, DiagnosticOptions::Options),
#define NOTE(ID, Options, Text, Signature)                                     \
  StoredDiagnosticInfo(DiagnosticKind::Note, DiagnosticOptions::Options),
#define REMARK(ID, Options, Text, Signature)                                   \
  StoredDiagnosticInfo(DiagnosticKind::Remark, DiagnosticOptions::Options),
#include "stone/Syntax/Diagnostics.def"
};
static_assert(sizeof(storedDiagnosticInfos) / sizeof(StoredDiagnosticInfo) ==
                  LocalDiagID::NumDiags,
              "array size mismatch");

static constexpr const char * const diagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text,
#include "stone/Syntax/Diagnostics.def"
    "<not a diagnostic>",
};



} // end anonymous namespace
