#include "stone/AST/ASTScope.h"

using namespace stone;

static const auto &GetScopeNameTable() {
  static const std::pair<ASTScopeKind, const char *> Table[] = {
      {ASTScopeKind::Decl, "Decl"},
      {ASTScopeKind::FunDecl, "Function Declaration"},
      {ASTScopeKind::ASTFile, "AST File"},
      {ASTScopeKind::FunctionSignature, "Function Signature "},
  };
  return Table;
}

const char *ASTScope::GetName(ASTScopeKind kind) {
  for (const auto &item : GetScopeNameTable()) {
    if (item.first == kind) {
      return item.second;
    }
  }
  assert(false && "Invalid ASTScopeKind");
}

ASTScope::ASTScope(ASTScopeKind kind, DiagnosticEngine &diags, ASTScope *parent)
    : kind(kind), diags(diags), parent(parent) {
  Initialize();
}
ASTScope::~ASTScope() {}

void ASTScope::Initialize() { scopeDecls.clear(); }