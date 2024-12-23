#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticBasicKind.h"
#include "stone/Diag/DiagnosticClient.h"

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
  constexpr StoredDiagnosticInfo(DiagnosticKind k, LocalDiagnosticOptions opts)
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
unsigned DiagnosticEngine::GetTotalDiagnostics() {
  return LocalDiagID::TotalDiags;
}

// TODO: categorization
static const constexpr StoredDiagnosticInfo storedDiagnosticInfos[] = {
#define ERROR(ID, Options, Message, Signature)                                 \
  StoredDiagnosticInfo(DiagnosticKind::Error, LocalDiagnosticOptions::Options),
#define WARNING(ID, Options, Message, Signature)                               \
  StoredDiagnosticInfo(DiagnosticKind::Warning,                                \
                       LocalDiagnosticOptions::Options),
#define NOTE(ID, Options, Message, Signature)                                  \
  StoredDiagnosticInfo(DiagnosticKind::Note, LocalDiagnosticOptions::Options),
#define REMARK(ID, Options, Message, Signature)                                \
  StoredDiagnosticInfo(DiagnosticKind::Remark, LocalDiagnosticOptions::Options),
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

DiagnosticState::DiagnosticState() {
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

DiagnosticEngine::DiagnosticEngine(SrcMgr &SM, DiagnosticOptions &DiagOpts)
    : SM(SM), DiagOpts(DiagOpts) {}

DiagnosticEngine::~DiagnosticEngine() {}

void DiagnosticEngine::AddClient(DiagnosticClient *client) {
  Clients.push_back(client);
}
std::vector<DiagnosticClient *> DiagnosticEngine::TakeClients() {
  auto clients =
      std::vector<DiagnosticClient *>(Clients.begin(), Clients.end());
  Clients.clear();
  return clients;
}

InFlightDiagnostic DiagnosticEngine::Diagnose(DiagID NextDiagID) {
  // return Diagnose(Diagnostic::Create(*this, NextDiagID));
}

InFlightDiagnostic DiagnosticEngine::Diagnose(SrcLoc NextDiagLoc,
                                              DiagID NextDiagID) {
  // return Diagnose(Diagnostic::Create(*this, NextDiagLoc, NextDiagID));
}
// InFlightDiagnostic
// DiagnosticEngine::Diagnose(DiagID NextDiagID, SrcLoc NextDiagLoc,
//                                   llvm::ArrayRef<DiagnosticArgument> Args) {
//   return Diagnose(Diagnostic::Create(*this, NextDiagID, NextDiagLoc, Args));
// }

InFlightDiagnostic DiagnosticEngine::Diagnose(const Diagnostic *diagnostic) {
  assert(!ActiveDiagnostic && "Already have an active diagnostic");
  return InFlightDiagnostic(*this);
}

InFlightDiagnostic &
InFlightDiagnostic::SetDiagnosticLevel(DiagnosticLevel Level) {
  DE->GetActiveDiagnostic()->SetLevel(Level);
  return *this;
}

InFlightDiagnostic &
InFlightDiagnostic::AddDiagnosticArgument(const DiagnosticArgument argument) {
  /// DE->GetActiveDiagnostic()->AddArg(std::move(argument));
  return *this;
}

void DiagnosticEngine::Clear(bool soft) {}

bool DiagnosticEngine::FinishProcessing() {
  // hasError
  // for (auto &Client : Clients) {
  //   hasError |= Client->FinishProcessing();
  // }
  // return hasError;
}

DiagnosticKind
DiagnosticEngine::DeclaredDiagnosticKindForDiagID(const DiagID ID) {
  return storedDiagnosticInfos[(unsigned)ID].kind;
}

llvm::StringRef
DiagnosticEngine::GetDiagnosticStringForDiagID(const DiagID ID,
                                               bool printDiagnosticNames) {
  return printDiagnosticNames ? debugDiagnosticStrings[(unsigned)ID]
                              : diagnosticStrings[(unsigned)ID];
}

llvm::StringRef
DiagnosticEngine::GetDiagnosticIDStringForDiagID(const DiagID ID) {
  return diagnosticIDStrings[(unsigned)ID];
}

// DiagnosticTracker::DiagnosticTracker() {
//   TotalWarnings.resize(LocalDiagID::TotalDiags);
//   TotalErrors.resize(LocalDiagID::TotalDiags);
// }

void InFlightDiagnostic::FlushActiveDiagnostic() {
  if (!IsActive) {
    return;
  }
  if (DE) {
    DE->FlushActiveDiagnostic(IsForceEmit);
  }
  Clear();
}

void DiagnosticEngine::FlushActiveDiagnostic(bool ForceEmit) {
  assert(ActiveDiagnostic && "No active diagnostic to flush");
  HandleDiagnostic(ActiveDiagnostic);
  ActiveDiagnostic = nullptr;
}

/// Handle a new diagnostic, which will either be emitted, or added to an
/// active transaction.
void DiagnosticEngine::HandleDiagnostic(const Diagnostic *diagnostic) {

  EmitDiagnostic(diagnostic);
}

void DiagnosticEngine::EmitDiagnostic(const Diagnostic *diagnostic) {
  assert(!HasClients() && "No DiagnosticClients. Unable to emit!");

  if (auto DI = ConstructDiagnosticImpl(diagnostic)) {
    for (auto &client : Clients) {
      if (client->UseInDiagnosticCounts()) {
        if (DI->IsWarning()) {
          state.TrackWarning(DI->ID);
        } else if (DI->IsError()) {
          state.TrackError(DI->ID);
        }
      }
    }
  }
  // Get the Level
}

static DiagnosticKind ComputeDiagnosticKind(DiagnosticLevel Level) {
  switch (Level) {
  case DiagnosticLevel::None:
    llvm_unreachable("unspecified diagnostic level");
  case DiagnosticLevel::Ignore:
    llvm_unreachable("trying to map an ignored diagnostic");
  case DiagnosticLevel::Error:
  case DiagnosticLevel::Fatal:
    return DiagnosticKind::Error;
  case DiagnosticLevel::Note:
    return DiagnosticKind::Note;
  case DiagnosticLevel::Warning:
    return DiagnosticKind::Warning;
  case DiagnosticLevel::Remark:
    return DiagnosticKind::Remark;
  }

  llvm_unreachable("Unhandled DiagnosticKind in switch.");
}

/// Generate DiagnosticInfo for a Diagnostic to be passed to consumers.
std::optional<DiagnosticImpl>
DiagnosticEngine::ConstructDiagnosticImpl(const Diagnostic *diagnostic) {

  auto Level = state.ComputeDiagnosticLevel(diagnostic);
  if (Level == DiagnosticLevel::Ignore) {
    return std::nullopt;
  }

  auto fixIts = diagnostic->GetFixIts();
  return DiagnosticImpl(
      diagnostic->GetID(), diagnostic->GetLoc(), ComputeDiagnosticKind(Level),
      /* None for now*/ DiagnosticReason::None,
      GetDiagnosticStringForDiagID(diagnostic->GetID()), diagnostic->GetArgs(),
      /* no children now*/ {}, diagnostic->GetRanges(), fixIts);
}

static DiagnosticLevel ComputeDiagnosticLevelImpl(DiagnosticKind kind,
                                                  bool isFatal) {
  switch (kind) {
  case DiagnosticKind::Note:
    return DiagnosticLevel::Note;
  case DiagnosticKind::Error:
    return isFatal ? DiagnosticLevel::Fatal : DiagnosticLevel::Error;
  case DiagnosticKind::Warning:
    return DiagnosticLevel::Warning;
  case DiagnosticKind::Remark:
    return DiagnosticLevel::Remark;
  }
  llvm_unreachable("Unhandled DiagnosticKind in switch.");
}

DiagnosticLevel
DiagnosticState::ComputeDiagnosticLevel(const Diagnostic *diag) {

  auto stroedDiagInfo = storedDiagnosticInfos[(unsigned)diag->GetID()];
  DiagnosticLevel Level = std::max(
      ComputeDiagnosticLevelImpl(stroedDiagInfo.kind, stroedDiagInfo.isFatal),
      diag->GetLevel());

  assert(Level != DiagnosticLevel::None);

  if (previousLevel == DiagnosticLevel::Ignore &&
      Level == DiagnosticLevel::Note) {
  }
  Level = DiagnosticLevel::Ignore;

  previousLevel = Level;
  return Level;
}

// Diagnostic *Diagnostic::Create(DiagnosticEngine &DE, DiagID ID) {
//   return Diagnostic::Create(DE, SrcLoc(), ID);
// }
// Diagnostic *Diagnostic::Create(DiagnosticEngine &DE, SrcLoc Loc, DiagID ID) {
//   return new (DE) Diagnostic(DE, ID, Loc);
// }
// Diagnostic *
// Diagnostic::Create(DiagnosticEngine &DE, DiagID ID,
//                           SrcLoc Loc,
//                           ArrayRef<DiagnosticArgument> Args) {
//   return new (DE) Diagnostic(ID, Loc, Args);
// }

// Diagnostic *Diagnostic::Create(DiagnosticEngine &DE, DiagID ID,
//                                              SrcLoc Loc,
//                                              ArrayRef<DiagnosticArgument>
//                                              Args, ArrayRef<FixIt> FixIts) {
//   return new (DE) Diagnostic(ID, Loc, Args, FixIts);
// }
