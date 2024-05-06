#include "stone/Parse/Parser.h"

using namespace stone;

// ==Scope == //

ParsingScope::ParsingScope(Parser &self, ScopeKind scopeKind,
                           llvm::StringRef description)
    : self(self), description(description) {
  EnterScope(scopeKind);
}
void ParsingScope::EnterScope(ScopeKind scopeKind) {
  self.EnterScope(scopeKind);
}
// Exit - Exit the scope associated with this object now, rather
// than waiting until the object is destroyed.
void ParsingScope::ExitScope() { self.ExitScope(); }

ParsingScope::~ParsingScope() { ExitScope(); }

// == Position RAII == //

ParsingPositionRAII::ParsingPositionRAII(Parser &parser)
    : parser(parser), parsingPos(parser.GetParsingPosition()) {}

ParsingPositionRAII::~ParsingPositionRAII() {
  parser.RestoreParsingPosition(parsingPos);
}

ParserStatus ParsingDecl::Verify() {
  ParserStatus status;
  return status;
}

ParsingDecl::~ParsingDecl() {

  // Apply();
}

bool ParsingDecl::IsAccessLevelActive(DeclSpecifierKind kind) {

  // TODO: List all kinds and throw if it is not an option.
  switch (kind) {
  case DeclSpecifierKind::Public:
  case DeclSpecifierKind::Private:
  case DeclSpecifierKind::Internal:
    return true;
  default:
    return false;
  }
}

bool ParsingDecl::IsTypeQualsActive(DeclSpecifierKind kind) {

  // TODO: List all kinds and throw if it is not an option.
  switch (kind) {
  case DeclSpecifierKind::Const:
  case DeclSpecifierKind::Pure:
  case DeclSpecifierKind::Mutable:
  case DeclSpecifierKind::Final:
    return true;
  default:
    return false;
  }
}

bool ParsingDecl::IsStorageTypeActive(DeclSpecifierKind kind) {

  // TODO: List all kinds and throw if it is not an option.
  switch (kind) {
  case DeclSpecifierKind::Static:
  case DeclSpecifierKind::Register:
  default:
    return false;
  }
}

ParsingPrettyStackTrace::ParsingPrettyStackTrace(Parser &parser)
    : parser(parser) {}

void ParsingPrettyStackTrace::print(llvm::raw_ostream &out) const {
  out << "With parser at source location: ";
  parser.GetCurTok().GetLoc().print(out, parser.GetASTContext().GetSrcMgr());
  out << '\n';
}
