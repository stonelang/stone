#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticClient.h"

using namespace stone;

// Get an official count of all of the diagnostics in the system
enum LocalDiagID : uint32_t {
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, SFINAE, NOWERROR,     \
             SHOWINSYSHEADER, SHOWINSYSMACRO, DEFERRABLE, CATEGORY)            \
  ENUM,
#include "stone/AST/AllDiagnosticKind.inc"
  TotalDiags
};

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

void diags::DiagnosticEngine::Clear(bool soft) {}

void diags::DiagnosticInfo::FormatDiagnostic(
    llvm::SmallVectorImpl<char> &OutStr) const {}

void diags::DiagnosticInfo::FormatDiagnostic(
    const char *DiagStr, const char *DiagEnd,
    llvm::SmallVectorImpl<char> &OutStr) const {

  // switch (Kind) {
  // }
}