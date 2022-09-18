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

SyntaxStatus ParsingDeclCollector::CollectUsing() {
  switch (GetParser().GetCurTok().GetKind()) {
  case tok::kw_using:
    GetUsingDeclarationCollector().AddUsing(GetParser().ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}

SyntaxStatus ParsingDeclCollector::CollectFunction() {
  switch (GetParser().GetCurTok().GetKind()) {
  case tok::kw_fun:
    GetFunctionSpecifierCollector().AddFun(GetParser().ConsumeToken());
    break;
  case tok::kw_inline:
    GetFunctionSpecifierCollector().AddInline(GetParser().ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus ParsingDeclCollector::CollectBasicType() {
  switch (GetParser().GetCurTok().GetKind()) {
  case tok::kw_auto:
    GetTypeSpecifierCollector().AddAuto(GetParser().ConsumeToken());
    break;
  case tok::kw_int:
    GetTypeSpecifierCollector().AddInt(GetParser().ConsumeToken());
    break;
  case tok::kw_int8:
    GetTypeSpecifierCollector().AddInt8(GetParser().ConsumeToken());
    break;
  case tok::kw_int16:
    GetTypeSpecifierCollector().AddInt16(GetParser().ConsumeToken());
    break;
  case tok::kw_int32:
    GetTypeSpecifierCollector().AddInt32(GetParser().ConsumeToken());
    break;
  case tok::kw_int64:
    GetTypeSpecifierCollector().AddInt64(GetParser().ConsumeToken());
    break;
  case tok::kw_uint:
    GetTypeSpecifierCollector().AddUInt(GetParser().ConsumeToken());
    break;
  case tok::kw_uint8:
    GetTypeSpecifierCollector().AddUInt8(GetParser().ConsumeToken());
    break;
  case tok::kw_byte:
    GetTypeSpecifierCollector().AddByte(GetParser().ConsumeToken());
    break;
  case tok::kw_uint16:
    GetTypeSpecifierCollector().AddUInt16(GetParser().ConsumeToken());
    break;
  case tok::kw_uint32:
    GetTypeSpecifierCollector().AddUInt32(GetParser().ConsumeToken());
    break;
  case tok::kw_uint64:
    GetTypeSpecifierCollector().AddUInt64(GetParser().ConsumeToken());
    break;
  case tok::kw_float:
    GetTypeSpecifierCollector().AddFloat(GetParser().ConsumeToken());
    break;
  case tok::kw_float32:
    GetTypeSpecifierCollector().AddFloat32(GetParser().ConsumeToken());
    break;
  case tok::kw_float64:
    GetTypeSpecifierCollector().AddFloat64(GetParser().ConsumeToken());
    break;
  case tok::kw_complex32:
    GetTypeSpecifierCollector().AddComplex32(GetParser().ConsumeToken());
    break;
  case tok::kw_complex64:
    GetTypeSpecifierCollector().AddComplex64(GetParser().ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus ParsingDeclCollector::CollectNominalType() {
  switch (GetParser().GetCurTok().GetKind()) {
  case tok::kw_enum:
    GetTypeSpecifierCollector().AddEnum(GetParser().ConsumeToken());
    break;
  case tok::kw_struct:
    GetTypeSpecifierCollector().AddStruct(GetParser().ConsumeToken());
    break;
  case tok::kw_interface:
    GetTypeSpecifierCollector().AddInterface(GetParser().ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus ParsingDeclCollector::CollectAccessLevel() {
  switch (GetParser().GetCurTok().GetKind()) {
  case tok::kw_public:
    GetAccessLevelCollector().AddPublic(GetParser().ConsumeToken());
    break;
  case tok::kw_internal:
    GetAccessLevelCollector().AddInternal(GetParser().ConsumeToken());
    break;
  case tok::kw_private:
    GetAccessLevelCollector().AddPrivate(GetParser().ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus ParsingDeclCollector::CollectTypeQualifier() {
  switch (GetParser().GetCurTok().GetKind()) {
  case tok::kw_const:
    GetTypeQualifierCollector().AddConst(GetParser().ConsumeToken());
    break;
  case tok::kw_restrict:
    GetTypeQualifierCollector().AddRestrict(GetParser().ConsumeToken());
    break;
  case tok::kw_volatile:
    GetTypeQualifierCollector().AddVolatile(GetParser().ConsumeToken());
    break;
  case tok::kw_pure:
    GetTypeQualifierCollector().AddPure(GetParser().ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus ParsingDeclCollector::CollectStorageSpecifier() {
  switch (GetParser().GetCurTok().GetKind()) {
  case tok::kw_static:
    GetStorageSpecifierCollector().AddStatic(GetParser().ConsumeToken());
    break;
  case tok::kw_register:
    GetStorageSpecifierCollector().AddRegister(GetParser().ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
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
