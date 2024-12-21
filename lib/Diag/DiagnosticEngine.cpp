#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticClient.h"

using namespace stone;

namespace {
// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum LocalDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/AST/Diagnostics.def"
  TotalDiags
};

} // namespace

diags::DiagnosticEngine::DiagnosticEngine(DiagnosticOptions &DiagOpts,
                                          SrcMgr &SM)
    : DiagOpts(DiagOpts), SM(SM) {}

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