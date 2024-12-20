#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticClient.h"

using namespace stone;

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



void diags::DiagnosticInfo::FormatDiagnostic(llvm::SmallVectorImpl<char> &OutStr) const{


}

void diags::DiagnosticInfo::FormatDiagnostic(const char *DiagStr, const char *DiagEnd,
                 llvm::SmallVectorImpl<char> &OutStr) const {


   //switch (Kind) {
    //}

}