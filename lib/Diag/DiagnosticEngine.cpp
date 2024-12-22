#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticBasicKind.h"
#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticFormatParser.h"

using namespace stone;

namespace {
enum class LocalDiagnosticOptions {
  /// No options.
  None,

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
  diags::DiagnosticKind kind : 2;
  bool pointsToFirstBadToken : 1;
  bool isFatal : 1;
  bool isAPIDigesterBreakage : 1;
  bool isDeprecation : 1;
  bool isNoUsage : 1;

  constexpr StoredDiagnosticInfo(diags::DiagnosticKind k, bool firstBadToken,
                                 bool fatal, bool isAPIDigesterBreakage,
                                 bool deprecation, bool noUsage)
      : kind(k), pointsToFirstBadToken(firstBadToken), isFatal(fatal),
        isAPIDigesterBreakage(isAPIDigesterBreakage),
        isDeprecation(deprecation), isNoUsage(noUsage) {}
  constexpr StoredDiagnosticInfo(diags::DiagnosticKind k,
                                 LocalDiagnosticOptions opts)
      : StoredDiagnosticInfo(
            k, opts == LocalDiagnosticOptions::PointsToFirstBadToken,
            opts == LocalDiagnosticOptions::Fatal,
            opts == LocalDiagnosticOptions::APIDigesterBreakage,
            opts == LocalDiagnosticOptions::Deprecation,
            opts == LocalDiagnosticOptions::NoUsage) {}
};

// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum LocalDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Message, Signature) ID,
#include "stone/Diag/DiagnosticEngine.def"
  TotalDiags
};
} // end anonymous namespace

/// Get the set of all diagnostic IDs.
unsigned diags::DiagnosticEngine::GetTotalDiagnostics() {
  return LocalDiagID::TotalDiags;
}

// TODO: categorization
static const constexpr StoredDiagnosticInfo storedDiagnosticInfos[] = {
#define ERROR(ID, Options, Message, Signature)                                 \
  StoredDiagnosticInfo(diags::DiagnosticKind::Error,                           \
                       LocalDiagnosticOptions::Options),
#define WARNING(ID, Options, Message, Signature)                               \
  StoredDiagnosticInfo(diags::DiagnosticKind::Warning,                         \
                       LocalDiagnosticOptions::Options),
#define NOTE(ID, Options, Message, Signature)                                  \
  StoredDiagnosticInfo(diags::DiagnosticKind::Note,                            \
                       LocalDiagnosticOptions::Options),
#define REMARK(ID, Options, Message, Signature)                                \
  StoredDiagnosticInfo(diags::DiagnosticKind::Remark,                          \
                       LocalDiagnosticOptions::Options),
#include "stone/Diag/DiagnosticEngine.def"
};

static_assert(sizeof(storedDiagnosticInfos) / sizeof(StoredDiagnosticInfo) ==
                  LocalDiagID::TotalDiags,
              "array size mismatch");

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

diags::DiagnosticState::DiagnosticState() {
  ignoredDiagnostics.resize(LocalDiagID::TotalDiags);
  errorDiagnostics.resize(LocalDiagID::TotalDiags);
  warningDiagnostics.resize(LocalDiagID::TotalDiags);
}
// static DiagnosticLevel
// DiagnosticState::ComputeDiagnosticLevel(DiagnosticKind kind, bool isFatal) {
//   switch (kind) {
//   case DiagnosticKind::Note:
//     return DiagnosticLevel::Note;
//   case DiagnosticKind::Error:
//     return isFatal ? DiagnosticLevel::Fatal : DiagnosticLevel::Error;
//   case DiagnosticKind::Warning:
//     return DiagnosticLevel::Warning;
//   case DiagnosticKind::Remark:
//     return DiagnosticLevel::Remark;
//   }
//   llvm_unreachable("Unhandled DiagnosticKind in switch.");
// }

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
  return Diagnose(NextDiagID, SrcLoc());
}

diags::InFlightDiagnostic
diags::DiagnosticEngine::Diagnose(DiagID NextDiagID, SrcLoc NextDiagLoc) {
  return Diagnose(NextDiagID, NextDiagLoc, {});
}
diags::InFlightDiagnostic
diags::DiagnosticEngine::Diagnose(DiagID NextDiagID, SrcLoc NextDiagLoc,
                                  llvm::ArrayRef<DiagnosticArgument> Args) {
  return Diagnose(Diagnostic::Create(*this, NextDiagID, NextDiagLoc, Args));
}

diags::InFlightDiagnostic
diags::DiagnosticEngine::Diagnose(const Diagnostic *diagnostic) {
  assert(!ActiveDiagnostic && "Already have an active diagnostic");
  return InFlightDiagnostic(*this);
}

diags::InFlightDiagnostic &
diags::InFlightDiagnostic::SetDiagnosticLevel(DiagnosticLevel Level) {
  DE->GetActiveDiagnostic()->SetLevel(Level);
  return *this;
}

void diags::DiagnosticEngine::Clear(bool soft) {}

bool diags::DiagnosticEngine::FinishProcessing() {
  // hasError
  // for (auto &Client : Clients) {
  //   hasError |= Client->FinishProcessing();
  // }
  // return hasError;
}

diags::DiagnosticKind
diags::DiagnosticEngine::DeclaredDiagnosticKindForDiagID(const DiagID ID) {
  return storedDiagnosticInfos[(unsigned)ID].kind;
}

llvm::StringRef diags::DiagnosticEngine::GetDiagnosticStringForDiagID(
    const DiagID ID, bool printDiagnosticNames) {
  return printDiagnosticNames ? debugDiagnosticStrings[(unsigned)ID]
                              : diagnosticStrings[(unsigned)ID];
}

llvm::StringRef
diags::DiagnosticEngine::GetDiagnosticIDStringForDiagID(const DiagID ID) {
  return diagnosticIDStrings[(unsigned)ID];
}

// diags::DiagnosticTracker::DiagnosticTracker() {
//   TotalWarnings.resize(LocalDiagID::TotalDiags);
//   TotalErrors.resize(LocalDiagID::TotalDiags);
// }

void diags::InFlightDiagnostic::FlushActiveDiagnostic() {
  if (!IsActive) {
    return;
  }
  if (DE) {
    DE->FlushActiveDiagnostic(IsForceEmit);
  }
  Clear();
}

void diags::DiagnosticEngine::FlushActiveDiagnostic(bool ForceEmit) {
  assert(ActiveDiagnostic && "No active diagnostic to flush");
  HandleDiagnostic(ActiveDiagnostic);
  ActiveDiagnostic = nullptr;
}

/// Handle a new diagnostic, which will either be emitted, or added to an
/// active transaction.
void diags::DiagnosticEngine::HandleDiagnostic(const Diagnostic *diagnostic) {

  EmitDiagnostic(diagnostic);
}

void diags::DiagnosticEngine::EmitDiagnostic(const Diagnostic *diagnostic) {
  assert(!HasClients() && "No DiagnosticClients. Unable to emit!");

  if (auto impl = ConstructDiagnosticImpl(diagnostic)) {

    // for (auto &consumer : Clients) {
    //   consumer->HandleDiagnostic(SourceMgr, *info);
    // }
  }
  // Get the Level
}

static diags::DiagnosticKind
ComputeDiagnosticKind(diags::DiagnosticLevel Level) {
  switch (Level) {
  case diags::DiagnosticLevel::None:
    llvm_unreachable("unspecified diagnostic level");
  case diags::DiagnosticLevel::Ignore:
    llvm_unreachable("trying to map an ignored diagnostic");
  case diags::DiagnosticLevel::Error:
  case diags::DiagnosticLevel::Fatal:
    return diags::DiagnosticKind::Error;
  case diags::DiagnosticLevel::Note:
    return diags::DiagnosticKind::Note;
  case diags::DiagnosticLevel::Warning:
    return diags::DiagnosticKind::Warning;
  case diags::DiagnosticLevel::Remark:
    return diags::DiagnosticKind::Remark;
  }

  llvm_unreachable("Unhandled DiagnosticKind in switch.");
}

/// Generate DiagnosticInfo for a Diagnostic to be passed to consumers.
std::optional<diags::DiagnosticImpl>
diags::DiagnosticEngine::ConstructDiagnosticImpl(const Diagnostic *diagnostic) {

  auto Level = state.ComputeDiagnosticLevel(diagnostic);
  if (Level == DiagnosticLevel::Ignore) {
    return std::nullopt;
  }

  return diags::DiagnosticImpl(ComputeDiagnosticKind(Level), diagnostic);
}

static diags::DiagnosticLevel
ComputeDiagnosticLevelImpl(diags::DiagnosticKind kind, bool isFatal) {
  switch (kind) {
  case diags::DiagnosticKind::Note:
    return diags::DiagnosticLevel::Note;
  case diags::DiagnosticKind::Error:
    return isFatal ? diags::DiagnosticLevel::Fatal
                   : diags::DiagnosticLevel::Error;
  case diags::DiagnosticKind::Warning:
    return diags::DiagnosticLevel::Warning;
  case diags::DiagnosticKind::Remark:
    return diags::DiagnosticLevel::Remark;
  }
  llvm_unreachable("Unhandled DiagnosticKind in switch.");
}

diags::DiagnosticLevel
diags::DiagnosticState::ComputeDiagnosticLevel(const Diagnostic *diag) {

  auto stroedDiagInfo = storedDiagnosticInfos[(unsigned)diag->GetID()];
  diags::DiagnosticLevel Level = std::max(
      ComputeDiagnosticLevelImpl(stroedDiagInfo.kind, stroedDiagInfo.isFatal),
      diag->GetLevel());

  assert(Level != DiagnosticLevel::None);

  if (previousLevel == diags::DiagnosticLevel::Ignore &&
      Level == diags::DiagnosticLevel::Note) {
  }
  Level = diags::DiagnosticLevel::Ignore;

  previousLevel = Level;
  return Level;
}

/// Format the given diagnostic text and place the result in the given
/// buffer.
void diags::DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef Text, SrcMgr &SM,
    DiagnosticFormatOptions FormatOpts) {

  FormatDiagnosticText(Out, Text, SM, {}, FormatOpts);
}

/// Format the given diagnostic text and place the result in the given
/// buffer.
void diags::DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef Text, SrcMgr &SM,
    ArrayRef<DiagnosticArgument> Args, DiagnosticFormatOptions FormatOpts) {

  // unsigned BufferID = SM.addMemBufferCopy(Text);
  // DiagnosticTextParser textParser(BufferID, SM);
  // textParser.Parse();
}

diags::Diagnostic *diags::Diagnostic::Create(diags::DiagnosticEngine &DE,
                                             diags::DiagID ID) {
  return new (DE) diags::Diagnostic(ID);
}
diags::Diagnostic *diags::Diagnostic::Create(diags::DiagnosticEngine &DE,
                                             diags::DiagID ID, SrcLoc Loc) {
  return new (DE) diags::Diagnostic(ID, Loc);
}
diags::Diagnostic *
diags::Diagnostic::Create(diags::DiagnosticEngine &DE, diags::DiagID ID,
                          SrcLoc Loc,
                          ArrayRef<diags::DiagnosticArgument> Args) {
  return new (DE) diags::Diagnostic(ID, Loc, Args);
}

diags::Diagnostic *diags::Diagnostic::Create(DiagnosticEngine &DE, DiagID ID,
                                             SrcLoc Loc,
                                             ArrayRef<DiagnosticArgument> Args,
                                             ArrayRef<FixIt> FixIts) {
  return new (DE) diags::Diagnostic(ID, Loc, Args, FixIts);
}
