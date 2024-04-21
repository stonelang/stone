#include "stone/Syntax/Diagnostics.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTPrinter.h"
#include "stone/Syntax/Decl.h"
// #include "stone/Syntax/DiagnosticSuppression.h"
#include "stone/Syntax/DiagnosticsBasic.h"
// #include "stone/Syntax/Expr.h"
#include "stone/Syntax/Module.h"
// #include "stone/Syntax/Pattern.h"
// #include "stone/Syntax/PrintOptions.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"

#include "stone/Basic/SrcMgr.h"
#include "stone/Support/LexerBase.h"

// #include "stone/Config.h"
//  #include "stone/Localization/LocalizationFormat.h"

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
        isAPIDigesterBreakage(isAPIDigesterBreakage),
        isDeprecation(deprecation), isNoUsage(noUsage) {}
  constexpr StoredDiagnosticInfo(DiagnosticKind k, diag::DiagnosticOptions opts)
      : StoredDiagnosticInfo(
            k, opts == diag::DiagnosticOptions::PointsToFirstBadToken,
            opts == diag::DiagnosticOptions::Fatal,
            opts == diag::DiagnosticOptions::APIDigesterBreakage,
            opts == diag::DiagnosticOptions::Deprecation,
            opts == diag::DiagnosticOptions::NoUsage) {}
};

// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum LocalDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/Syntax/Diagnostics.def"
  NumDiags
};

} // namespace

// TODO: categorization
static const constexpr StoredDiagnosticInfo storedDiagnosticInfos[] = {
#define ERROR(ID, Options, Text, Signature)                                    \
  StoredDiagnosticInfo(DiagnosticKind::Error, diag::DiagnosticOptions::Options),
#define WARNING(ID, Options, Text, Signature)                                  \
  StoredDiagnosticInfo(DiagnosticKind::Warning,                                \
                       diag::DiagnosticOptions::Options),
#define NOTE(ID, Options, Text, Signature)                                     \
  StoredDiagnosticInfo(DiagnosticKind::Note, diag::DiagnosticOptions::Options),
#define REMARK(ID, Options, Text, Signature)                                   \
  StoredDiagnosticInfo(DiagnosticKind::Remark,                                 \
                       diag::DiagnosticOptions::Options),
#include "stone/Syntax/Diagnostics.def"
};
static_assert(sizeof(storedDiagnosticInfos) / sizeof(StoredDiagnosticInfo) ==
                  LocalDiagID::NumDiags,
              "array size mismatch");

static constexpr const char *const diagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text,
#include "stone/Syntax/Diagnostics.def"
    "<not a diagnostic>",
};

static constexpr const char *const debugDiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text " [" #ID "]",
#include "stone/Syntax/Diagnostics.def"
    "<not a diagnostic>",
};

static constexpr const char *const diagnosticIDStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) #ID,
#include "stone/Syntax/Diagnostics.def"
    "<not a diagnostic>",
};

static constexpr const char *const fixItStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature)
#define FIXIT(ID, Text, Signature) Text,
#include "stone/Syntax/Diagnostics.def"
    "<not a fix-it>",
};

diag::DiagnosticState::DiagnosticState() {
  // Initialize our ignored diagnostics to default
  ignoredDiagnostics.resize(LocalDiagID::NumDiags);
}

// static CharSrcRange toCharSrcRange(SrcMgr &SM, SrcRange SR) {
//   return CharSrcRange(SM, SR.Start, Lexer::getLocForEndOfToken(SM, SR.End));
// }

// static CharSrcRange toCharSrcRange(SrcMgr &SM, SrcLoc Start,
//                                          SrcLoc End) {
//   return CharSrcRange(SM, Start, End);
// }

/// Extract a character at \p Loc. If \p Loc is the end of the buffer,
/// return '\f'.
static char extractCharAfter(SrcMgr &SM, SrcLoc Loc) {
  auto chars = SM.extractText({Loc, 1});
  return chars.empty() ? '\f' : chars[0];
}

/// Extract a character immediately before \p Loc. If \p Loc is the
/// start of the buffer, return '\f'.
static char extractCharBefore(SrcMgr &SM, SrcLoc Loc) {
  // We have to be careful not to go off the front of the buffer.
  auto bufferID = SM.findBufferContainingLoc(Loc);
  auto bufferRange = SM.getRangeForBuffer(bufferID);
  if (bufferRange.getStart() == Loc)
    return '\f';
  auto chars = SM.extractText({Loc.getAdvancedLoc(-1), 1}, bufferID);
  assert(!chars.empty() && "Couldn't extractText with valid range");
  return chars[0];
}

diag::InFlightDiagnostic &diag::InFlightDiagnostic::highlight(SrcRange R) {
  assert(IsActive && "Cannot modify an inactive diagnostic");
  if (Engine && R.isValid())
    Engine->getActiveDiagnostic().addRange(CharSrcRange(
        Engine->SourceMgr, R.Start, lexerBase->GetLocForEndOfToken(Engine->SourceMgr, R.End)));
  return *this;
}

diag::InFlightDiagnostic &diag::InFlightDiagnostic::highlightChars(SrcLoc Start,
                                                             SrcLoc End) {
  assert(IsActive && "Cannot modify an inactive diagnostic");
  if (Engine && Start.isValid())
    Engine->getActiveDiagnostic().addRange(
        CharSrcRange(Engine->SourceMgr, Start, End));
  return *this;
}

/// Add an insertion fix-it to the currently-active diagnostic.  The
/// text is inserted immediately *after* the token specified.
///
diag::InFlightDiagnostic &
diag::InFlightDiagnostic::fixItInsertAfter(SrcLoc L, StringRef FormatString,
                                           ArrayRef<DiagnosticArgument> Args) {
  L = lexerBase->GetLocForEndOfToken(Engine->SourceMgr, L);
  return fixItInsert(L, FormatString, Args);
}
