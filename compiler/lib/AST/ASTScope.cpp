#include "stone/AST/ASTScope.h"

using namespace stone;

static const auto &GetScopeNameTable() {
  static const std::pair<ScopeKind, const char *> Table[] = {
      {ScopeKind::Decl, "Decl"},
      {ScopeKind::FunDecl, "Function Declaration"},
      {ScopeKind::ASTFile, "AST File"},
      {ScopeKind::FunctionSignature, "Function Signature "},
  };
  return Table;
}

const char *ASTScope::GetName(ScopeKind kind) {
  for (const auto &item : GetScopeNameTable()) {
    if (item.first == kind) {
      return item.second;
    }
  }
  assert(false && "Invalid ScopeKind");
}

ASTScope::ASTScope(ASTScopeKind kind, DiagnosticEngine &diags, Scope *parent)
    : kind(kind), diags(diags), parent(parent) {
  Initialize();
}
ASTScope::~ASTScope() {}

void ASTScope::Initialize() { scopeDecls.clear(); }