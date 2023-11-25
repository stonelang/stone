#include "stone/Parse/Parser.h"
#include "stone/Basic/Mem.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/ASTDiagnostic.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Scope.h"

using namespace stone;

Parser::Parser(SourceFile &sf, ASTContext &astContext)
    : Parser(sf, astContext,
             std::unique_ptr<Lexer>(
                 new Lexer(sf.GetSrcID(), astContext.GetSrcMgr(),
                           &astContext.GetDiags(), &astContext.GetStats()))) {}

Parser::Parser(SourceFile &sf, ASTContext &astContext,
               std::unique_ptr<Lexer> lx)
    : sf(sf), astContext(astContext), lexer(lx.release()), curDC(&sf),
      stats(new ParserStats(*this)) {

  astContext.GetStats().Register(stats.get());
}

Parser::~Parser() {}

// Scope *Parser::GetCurScope() const {
//   assert(false && "Not implemented");
//   return nullptr;
// }

// void Parser::EnterScope(ScopeKind scopeKind) {}
// void Parser::ExitScope() {}

SrcLoc Parser::ConsumeToken(ParsingNotification notification) {
  auto loc = curTok.GetLoc();
  assert(curTok.IsNot(tok::eof) && "Lexing past eof!");

  if (notification == ParsingNotification::TokenConsumed) {
    if (lexerListener) {
      lexerListener->OnToken(&curTok);
    }
  }
  Lex(curTok, leadingTrivia, trailingTrivia);
  prevTokLoc = loc;
  return loc;
}

// static bool HasFlagsSet(Parser::SkipToFlags L, Parser::SkipToFlags R) {
//   return (static_cast<unsigned>(L) & static_cast<unsigned>(R)) != 0;
// }

// bool Parser::SkipTo(llvm::ArrayRef<tok> toks, SkipToFlags flags) {
//   // We always want this function to skip at least one token if the first
//   token
//   // isn't T and if not at EOF.
//   bool isFirstTokenSkipped = true;
//   while (true) {
//     // If we found one of the tokens, stop and return true.
//     for (unsigned i = 0, numToks = toks.size(); i != numToks; ++i) {
//       if (token.Is(toks[i])) {
//         if (HasFlagsSet(flags, StopBeforeMatch)) {
//           // Noop, don't consume the token.
//         } else {
//           ConsumeAnyTok();
//         }
//         return true;
//       }
//     }
//     return false;
//   }
// }

/// Keeping this simple for now
// void Parser::ParseDeclName(DeclNameContext &nameResult) {
//   // Parse function name.
//   auto name = GetIdentifier(curTok.GetText());
//   nameResult.SetName(DeclName(&name));
//   nameResult.SetNameLoc(ConsumeToken(tok::identifier));
// }

// This is there because you may want to strip certain things from the
// identifier name -- something to think about.
Identifier Parser::GetIdentifier(llvm::StringRef text) {
  return astContext.GetIdentifier(text);
}

SrcLoc Parser::ConsumeStartingCharOfCurToken(tok kind, size_t len) {
  // Consumes prefix of token and returns its location.
  // (like '?', '<', '>' or '!' immediately followed by '<')
  assert(len >= 1);

  // Current token can be either one-character token we want to consume...
  // if (Tok.getLength() == Len) {
  //   Tok.setKind(Kind);
  //   return consumeToken();
  // }

  // auto Loc = Tok.getLoc();

  // // ... or a multi-character token with the first N characters being the one
  // // that we want to consume as a separate token.
  // assert(Tok.getLength() > Len);
  // markSplitToken(Kind, Tok.getText().substr(0, Len));

  // auto NewState = L->getStateForBeginningOfTokenLoc(Loc.getAdvancedLoc(Len));
  // restoreParserPosition(ParserPosition(NewState, Loc),
  //                       /*enableDiagnostics=*/true);
  // return PreviousLoc;
  SrcLoc();
}

SyntaxStatus Parser::ParseIdentifier(Identifier &result, SrcLoc &resultLoc) {
  SyntaxStatus status;

  assert(GetTok().IsIdentifierOrUnderscore());
  resultLoc = ConsumeIdentifier(result);

  return status;
}
SrcLoc Parser::ConsumeIdentifier(Identifier &result) {
  // assert(Tok.isAny(tok::identifier, tok::kw_self, tok::kw_Self));
  // assert(Result.empty());
  result = GetIdentifier(GetTok().GetText());
  // if (Tok.getText()[0] == '$')
  //   diagnoseDollarIdentifier(Tok, diagnoseDollarPrefix);
  return ConsumeToken();
}

void Parser::RecordTokenHash(llvm::StringRef tokText) {
  assert(!tokText.empty());
  if (currentTokenHash) {
    currentTokenHash->combine(tokText);
    // Add null byte to separate tokens.
    currentTokenHash->combine(UInt8{0});
  }
}
SrcLoc Parser::ConsumeStartingLess() {
  assert(StartsWithLess(curTok) && "Token does not start with '<'");
  return ConsumeStartingCharOfCurToken(tok::l_angle);
}

SrcLoc Parser::ConsumeStartingGreater() {
  assert(StartsWithGreater(curTok) && "Token does not start with '>'");
  return ConsumeStartingCharOfCurToken(tok::r_angle);
}

/// EnterScope - start a new scope.
void Parser::EnterScope(ScopeKind kind) {

  if (!GetCurScope()) {
    assert(kind == ScopeKind::TopLevel);
  }
  // Create the new scope
  auto curScope = CreateScope(kind, GetCurScope());

  // Make sure we have a scope
  assert(curScope);

  // Cache the scope
  PushCurScope(curScope);
}

Scope *Parser::CreateScope(ScopeKind kind, Scope *parent) {
  return Parser::CreateScope(kind, GetASTContext(), GetDiags(), parent);
}
/// ExitScope - pop a scope off the scope stack.
void Parser::ExitScope() {

  // Ensure we have a current scope.
  if (GetCurScope()) {
    // Remove the scope
    PopCurScope();
  }
}
Scope *Parser::CreateScope(ScopeKind kind, ASTContext &ac,
                           DiagnosticEngine &diags, Scope *parent) {
  return new (ac) Scope(kind, diags, parent);
}

InFlightDiagnostic Parser::PrintD(SrcLoc loc, Diag<> diagID) {
  return astContext.GetDiags().PrintD(loc, diagID);
}

InFlightDiagnostic Parser::PrintD(Token &token, Diag<> diagID) {
  return PrintD(token.GetLoc(), diagID);
}

void ParserStats::Print(ColorStream &stream) {}
