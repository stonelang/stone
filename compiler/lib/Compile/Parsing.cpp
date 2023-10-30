#include "stone/Parse/Parser.h"

using namespace stone;
using namespace stone::syn;

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

ASTStatus ParsingDeclCollector::Verify() {
  ASTStatus status;
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

bool ParsingToken::IsPeriod() {
  return parser.GetTok().GetKind() == tok::period;
}
bool ParsingToken::IsDoublePipe() {
  return parser.GetTok().GetKind() == tok::doublepipe;
}
bool ParsingToken::IsPipe() { return parser.GetTok().GetKind() == tok::pipe; }
bool ParsingToken::IsPipeEqual() {
  return parser.GetTok().GetKind() == tok::pipeequal;
}
bool ParsingToken::IsEllipsis() {
  return parser.GetTok().GetKind() == tok::ellipsis;
}
bool ParsingToken::IsSemi() { return parser.GetTok().GetKind() == tok::semi; }
bool ParsingToken::IsEquality() {
  return parser.GetTok().GetKind() == tok::equal;
}
bool ParsingToken::IsDoubleEquality() {
  return parser.GetTok().GetKind() == tok::doubleequal;
}
bool ParsingToken::IsPound() { return parser.GetTok().GetKind() == tok::pound; }
bool ParsingToken::IsAmp() { return parser.GetTok().GetKind() == tok::amp; }
bool ParsingToken::IsArrow() { return parser.GetTok().GetKind() == tok::arrow; }
bool ParsingToken::IsBackTick() {
  return parser.GetTok().GetKind() == tok::backtick;
}
bool ParsingToken::IsExcliam() {
  return parser.GetTok().GetKind() == tok::exclaim;
}
bool ParsingToken::IsDoubleColon() {
  return parser.GetTok().GetKind() == tok::doublecolon;
}
bool ParsingToken::IsTilde() { return parser.GetTok().GetKind() == tok::tilde; }
bool ParsingToken::IsFun() { return parser.GetTok().GetKind() == tok::kw_fun; }
bool ParsingToken::IsStruct() {
  return parser.GetTok().GetKind() == tok::kw_struct;
}
bool ParsingToken::IsInterface() {
  return parser.GetTok().GetKind() == tok::kw_interface;
}
bool ParsingToken::IsPure() {
  return parser.GetTok().GetKind() == tok::kw_pure;
}
bool ParsingToken::IsInline() {
  return parser.GetTok().GetKind() == tok::kw_pure;
}
bool ParsingToken::IsLeftParen() {
  return parser.GetTok().GetKind() == tok::l_paren;
}
bool ParsingToken::IsRightParen() {
  return parser.GetTok().GetKind() == tok::r_paren;
}
bool ParsingToken::IsEnum() {
  return parser.GetTok().GetKind() == tok::kw_enum;
}
bool ParsingToken::IsStar() { return parser.GetTok().GetKind() == tok::star; }
bool ParsingToken::IsQualifier() {
  return parser.GetTok().IsAny(tok::kw_const, tok::kw_restrict,
                               tok::kw_volatile, tok::kw_pure);
}
bool ParsingToken::IsAccessLevel() {
  return parser.GetTok().IsAny(tok::kw_public, tok::kw_internal,
                               tok::kw_private);
}
