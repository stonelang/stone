#include "stone/Parse/Parser.h"
#include "stone/Syntax/Pattern.h"

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

SyntaxStatus ParsingDeclCollector::Verify() {
  SyntaxStatus status;
  return status;
}

ParsingDeclCollector::~ParsingDeclCollector() {}

ParsingPrettyStackTrace::ParsingPrettyStackTrace(Parser &parser)
    : parser(parser) {}

void ParsingPrettyStackTrace::print(llvm::raw_ostream &out) const {
  out << "With parser at source location: ";
  parser.GetCurTok().GetLoc().print(out, parser.GetSyntaxContext().GetSrcMgr());
  out << '\n';
}

bool ParsingToken::IsPeriod() { return parser.curTok.GetKind() == tok::period; }
bool ParsingToken::IsDoublePipe() {
  return parser.curTok.GetKind() == tok::doublepipe;
}
bool ParsingToken::IsPipe() { return parser.curTok.GetKind() == tok::pipe; }
bool ParsingToken::IsPipeEqual() {
  return parser.curTok.GetKind() == tok::pipeequal;
}
bool ParsingToken::IsEllipsis() {
  return parser.curTok.GetKind() == tok::ellipsis;
}
bool ParsingToken::IsSemi() { return parser.curTok.GetKind() == tok::semi; }
bool ParsingToken::IsEquality() {
  return parser.curTok.GetKind() == tok::equal;
}
bool ParsingToken::IsDoubleEquality() {
  return parser.curTok.GetKind() == tok::doubleequal;
}
bool ParsingToken::IsPound() { return parser.curTok.GetKind() == tok::pound; }
bool ParsingToken::IsAmp() { return parser.curTok.GetKind() == tok::amp; }
bool ParsingToken::IsArrow() { return parser.curTok.GetKind() == tok::arrow; }
bool ParsingToken::IsBackTick() {
  return parser.curTok.GetKind() == tok::backtick;
}
bool ParsingToken::IsExcliam() {
  return parser.curTok.GetKind() == tok::exclaim;
}
bool ParsingToken::IsDoubleColon() {
  return parser.curTok.GetKind() == tok::doublecolon;
}
bool ParsingToken::IsTilde() { return parser.curTok.GetKind() == tok::tilde; }
bool ParsingToken::IsFun() { return parser.curTok.GetKind() == tok::kw_fun; }
bool ParsingToken::IsStruct() {
  return parser.curTok.GetKind() == tok::kw_struct;
}
bool ParsingToken::IsInterface() {
  return parser.curTok.GetKind() == tok::kw_interface;
}
bool ParsingToken::IsPure() { return parser.curTok.GetKind() == tok::kw_pure; }
bool ParsingToken::IsInline() {
  return parser.curTok.GetKind() == tok::kw_pure;
}
bool ParsingToken::IsLeftParen() {
  return parser.curTok.GetKind() == tok::l_paren;
}
bool ParsingToken::IsRightParen() {
  return parser.curTok.GetKind() == tok::r_paren;
}
bool ParsingToken::IsEnum() { return parser.curTok.GetKind() == tok::kw_enum; }
bool ParsingToken::IsStar() { return parser.curTok.GetKind() == tok::star; }
bool ParsingToken::IsQualifier() {
  return parser.curTok.IsAny(tok::kw_const, tok::kw_restrict, tok::kw_volatile,
                             tok::kw_pure);
}
bool ParsingToken::IsAccessLevel() {
  return parser.curTok.IsAny(tok::kw_public, tok::kw_internal, tok::kw_private);
}
