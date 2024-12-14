#include "stone/AST/ASTScope.h"

using namespace stone;

// static const auto &GetScopeNameTable() {
//   static const std::pair<ScopeKind, const char *> Table[] = {
//       {ScopeKind::Decl, "Decl"},
//       {ScopeKind::FunDecl, "Function Declaration"},
//       {ScopeKind::SourceFile, "Source File"},
//       {ScopeKind::FunctionSignature, "Function Signature "},
//   };
//   return Table;
// }

// const char *Scope::GetName(ScopeKind kind) {
//   for (const auto &item : GetScopeNameTable()) {
//     if (item.first == kind) {
//       return item.second;
//     }
//   }
//   assert(false && "Invalid ScopeKind");
// }

// Scope::Scope(ScopeKind kind, DiagnosticEngine &diags, Scope *parent)
//     : kind(kind), diags(diags), parent(parent) {
//   Initialize();
// }
// Scope::~Scope() {}

// void Scope::Initialize() { scopeDecls.clear(); }