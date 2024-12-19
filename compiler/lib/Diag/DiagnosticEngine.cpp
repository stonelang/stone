#include "stone/Diag/DiagnosticEngine.h"

using namespace stone;

diags::DiagnosticEngine::DiagnosticEngine(DiagnosticOptions &DiagOpts,
                                          SrcMgr &SM)
    : DiagOpts(DiagOpts), SM(SM) {}

diags::DiagnosticEngine::~DiagnosticEngine() { SetClient(nullptr); }

void diags::DiagnosticEngine::SetClient(diags::DiagnosticClient *client) {
  ClientOwner.reset(!client->HasDefaultOwnership() ? client : nullptr);
  Client = client;
}
std::unique_ptr<diags::DiagnosticClient> diags::DiagnosticEngine::TakeClient() {
  ClientOwner->SetOwnership(diags::DiagnosticClient::Ownership::Default);
  return std::move(ClientOwner);
}

void diags::DiagnosticEngine::Clear(bool soft) {}