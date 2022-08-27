#include "stone/Parse/Parser.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Context.h"
#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxScope.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(SyntaxFile &sf, Syntax &syntax, SyntaxListener *listener)
    : Parser(sf, syntax,
             std::unique_ptr<Lexer>(new Lexer(
                 sf.GetSrcID(), syntax.GetSyntaxContext().GetSrcMgr(),
                 &syntax.GetSyntaxContext()
                      .GetContext()
                      .GetDiagUnit()
                      .GetDiagEngine(),
                 &syntax.GetSyntaxContext().GetContext().GetStatEngine())),
             listener) {}

Parser::Parser(SyntaxFile &sf, Syntax &syntax, std::unique_ptr<Lexer> lx,
               SyntaxListener *listener)
    : sf(sf), syntax(syntax), lexer(lx.release()), curDC(&sf),
      listener(listener) {

  stats.reset(new ParserStats(*this));
  GetContext().GetStatEngine().Register(stats.get());
}

Parser::~Parser() {}

// SyntaxScope *Parser::GetCurScope() const {
//   assert(false && "Not implemented");
//   return nullptr;
// }

bool Parser::HasError() { return GetContext().GetDiagUnit().HasError(); }
Context &Parser::GetContext() { return syntax.GetSyntaxContext().GetContext(); }

// void Parser::EnterScope(SyntaxScopeKind scopeKind) {}
// void Parser::ExitScope() {}

SrcLoc Parser::ConsumeToken(Parser::Notification notification) {
  auto loc = token.GetLoc();
  assert(token.IsNot(tok::eof) && "Lexing past eof!");

  if ((notification == Parser::Notification::TokenConsumed) && listener) {
    listener->OnToken(&token);
  }
  Lex(token, leadingTrivia, trailingTrivia);
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

// This is there because you may want to strip certain things from the
// identifier name -- something to think about.
Identifier &Parser::GetIdentifierOnly(llvm::StringRef text) {
  return syntax.MakeIdentifier(text);
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

SrcLoc Parser::ConsumeStartingLess() {
  assert(StartsWithLess(token) && "Token does not start with '<'");
  return ConsumeStartingCharOfCurToken(tok::l_angle);
}

SrcLoc Parser::ConsumeStartingGreater() {
  assert(StartsWithGreater(token) && "Token does not start with '>'");
  return ConsumeStartingCharOfCurToken(tok::r_angle);
}

InFlightDiagnostic Parser::PrintD(SrcLoc loc, Diag<> diagID) {
  return GetContext().GetDiagUnit().PrintD(loc, diagID);
}

InFlightDiagnostic Parser::PrintD(Token &token, Diag<> diagID) {
  return PrintD(token.GetLoc(), diagID);
}

void ParserStats::Print(ColorfulStream &stream) {}
