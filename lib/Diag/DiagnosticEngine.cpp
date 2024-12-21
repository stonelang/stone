#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticBasicKind.h"
#include "stone/Diag/DiagnosticClient.h"

using namespace stone;

namespace {

// Maps to the Options element in the .def file
enum class StoredDiagnosticOptions {
  /// No options.
  None = 0,
  /// The location of this diagnostic points to the beginning of the first
  /// token that the parser considers invalid.  If this token is located at the
  /// beginning of the line, then the location is adjusted to point to the end
  /// of the previous token.
  ///
  /// This behavior improves experience for "expected token X" diagnostics.
  FirstBadToken,

  /// After a fatal error subsequent diagnostics are suppressed.
  Fatal,

  /// A deprecation warning or error.
  Deprecation,

  /// A diagnostic warning about an unused element.
  NotUsed,
};

struct StoredDiagnosticInfo {
  diags::DiagnosticKind kind : 2;
  bool firstBadToken : 1;
  bool isFatal : 1;
  bool isDeprecation : 1;
  bool isNotUsed : 1;

  constexpr StoredDiagnosticInfo(diags::DiagnosticKind k, bool firstBadToken,
                                 bool isFatal, bool isDeprecation, bool notUsed)
      : kind(k), firstBadToken(firstBadToken), isFatal(isFatal),
        isDeprecation(isDeprecation), isNotUsed(isNotUsed) {}

  constexpr StoredDiagnosticInfo(diags::DiagnosticKind k,
                                 StoredDiagnosticOptions opts)
      : StoredDiagnosticInfo(k, opts == StoredDiagnosticOptions::FirstBadToken,
                             opts == StoredDiagnosticOptions::Fatal,
                             opts == StoredDiagnosticOptions::Deprecation,
                             opts == StoredDiagnosticOptions::NotUsed) {}
};

// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum StoredDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Message, Signature) ID,
#include "stone/Diag/DiagnosticEngine.def"
  TotalDiags
};

} // namespace

// static const constexpr StoredDiagnosticInfo storedDiagnosticInfos[] = {
// #define ERROR(ID, Options, Message, Signature) \
//   StoredDiagnosticInfo(diags::DiagnosticKind::Error,
//   StoredDiagnosticOptions::Options),
// #define WARNING(ID, Options, Message, Signature) \
//   StoredDiagnosticInfo(diags::DiagnosticKind::Warning, \
//                        StoredDiagnosticOptions::Options),
// #define NOTE(ID, Options, Message, Signature) \
//   StoredDiagnosticInfo(diags::DiagnosticKind::Note,
//   StoredDiagnosticOptions::Options),
// #define REMARK(ID, Options, Message, Signature) \
//   StoredDiagnosticInfo(diags::DiagnosticKind::Remark,
//   StoredDiagnosticOptions::Options),
// #include "stone/Diag/DiagnosticEngine.def"
// };

// static_assert(sizeof(storedDiagnosticInfos) / sizeof(StoredDiagnosticInfo) ==
//                   StoredDiagID::TotalDiags,
//               "array size mismatch");

static constexpr const char *const diagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Message, Signature) Message,
#include "stone/Diag/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const debugDiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Message, Signature) Message " [" #ID "]",
#include "stone/Diag/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const diagnosticIDStrings[] = {
#define DIAG(KIND, ID, Options, Message, Signature) #ID,
#include "stone/Diag/DiagnosticEngine.def"
    "<not a diagnostic>",
};

// static constexpr const char *const fixItStrings[] = {
// #define DIAG(KIND, ID, Options, Message, Signature)
// #define FIXIT(ID, Message, Signature) Message,
// #include "stone/Diag/DiagnosticEngine.def"
//     "<not a fix-it>",
// };

diags::DiagnosticEngine::DiagnosticEngine(SrcMgr &SM,
                                          DiagnosticOptions &DiagOpts)
    : SM(SM), DiagOpts(DiagOpts) {}

diags::DiagnosticEngine::~DiagnosticEngine() {}

void diags::DiagnosticEngine::AddClient(diags::DiagnosticClient *client) {
  Clients.push_back(client);
}
std::vector<diags::DiagnosticClient *> diags::DiagnosticEngine::TakeClients() {
  auto clients =
      std::vector<diags::DiagnosticClient *>(Clients.begin(), Clients.end());
  Clients.clear();
  return clients;
}

diags::InFlightDiagnostic diags::DiagnosticEngine::Diagnose(DiagID NextDiagID) {
  return Diagnose(SrcLoc(), NextDiagID);
}

diags::InFlightDiagnostic diags::DiagnosticEngine::Diagnose(SrcLoc NextDiagLoc,
                                                            DiagID NextDiagID) {

  assert(CurDiagID == std::numeric_limits<DiagID>::max() &&
         "Multiple diagnostics in flight at once!");

  CurDiagLoc = NextDiagLoc;
  CurDiagID = NextDiagID;
  // FlagValue.clear();
  return InFlightDiagnostic(this);
}

void diags::DiagnosticEngine::Clear(bool soft) {}

void diags::DiagnosticEngine::FinishProcessing() {}

void diags::DiagnosticInfo::FormatDiagnostic(
    llvm::SmallVectorImpl<char> &OutStr) const {}

void diags::DiagnosticInfo::FormatDiagnostic(
    const char *DiagStr, const char *DiagEnd,
    llvm::SmallVectorImpl<char> &OutStr) const {

  // switch (Kind) {
  // }
}