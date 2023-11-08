#include "stone/CodeAna/Parser.h"

using namespace stone;

// ==Scope == //

ParsingScope::ParsingScope(Parser &self, ASTScopeKind scopeKind,
                           llvm::StringRef description)
    : self(self), description(description) {
  EnterScope(scopeKind);
}
void ParsingScope::EnterScope(ASTScopeKind scopeKind) {
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

ParserStatus ParsingDeclCollector::Verify() {
  ParserStatus status;
  return status;
}

ParsingDeclCollector::~ParsingDeclCollector() { Apply(); }

ParsingPrettyStackTrace::ParsingPrettyStackTrace(Parser &parser)
    : parser(parser) {}

void ParsingPrettyStackTrace::print(llvm::raw_ostream &out) const {
  out << "With parser at source location: ";
  parser.GetTok().GetLoc().print(out, parser.GetASTContext().GetSrcMgr());
  out << '\n';
}
