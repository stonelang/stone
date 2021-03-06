#include "stone/Parse/Parser.h"
#include "stone/Basic/Basic.h"
#include "stone/Basic/Ret.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/SyntaxDiagnostic.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxScope.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(SyntaxFile &sf, Syntax &syntax, SyntaxListener *listener)
    : Parser(sf, syntax,
             std::unique_ptr<Lexer>(
                 new Lexer(sf.GetSrcID(), syntax.GetTreeContext().GetSrcMgr(),
                           syntax.GetTreeContext().GetBasic())),
             listener) {}

Parser::Parser(SyntaxFile &sf, Syntax &syntax, std::unique_ptr<Lexer> lx,
               SyntaxListener *listener)
    : sf(sf), syntax(syntax), lexer(lx.release()), curDC(&sf),
      listener(listener) {

  stats.reset(new ParserStats(*this, GetBasic()));
  GetBasic().GetStatEngine().Register(stats.get());

  Init();
}
void Parser::Init() {

  // Create the translation unit scope.  Install it as the current scope.
  // assert(GetCurScope() == nullptr && "A scope is already active?");
  // Prime the Lexer
  // ConsumeTok();
}

Parser::~Parser() {}

SyntaxScope *Parser::GetCurScope() const {
  assert(false && "Not implemented");
  return nullptr;
}

bool Parser::HasError() { return GetBasic().HasError(); }
Basic &Parser::GetBasic() { return syntax.GetTreeContext().GetBasic(); }

void Parser::EnterScope(unsigned scopeFlags) {}
void Parser::ExitScope() {}

SrcLoc Parser::ConsumeTok(bool onTok) {
  SrcLoc loc = tok.GetLoc();
  assert(tok.IsNot(tk::Type::eof) && "Lexing past eof!");
  Lex(tok, leadingTrivia, trailingTrivia);
  prevTokLoc = loc;
  return loc;
}

static bool HasFlagsSet(Parser::SkipToFlags L, Parser::SkipToFlags R) {
  return (static_cast<unsigned>(L) & static_cast<unsigned>(R)) != 0;
}
bool Parser::SkipTo(llvm::ArrayRef<tk::Type> toks, SkipToFlags flags) {
  // We always want this function to skip at least one token if the first token
  // isn't T and if not at EOF.
  bool isFirstTokenSkipped = true;
  while (true) {
    // If we found one of the tokens, stop and return true.
    for (unsigned i = 0, numToks = toks.size(); i != numToks; ++i) {
      if (tok.Is(toks[i])) {
        if (HasFlagsSet(flags, StopBeforeMatch)) {
          // Noop, don't consume the token.
        } else {
          ConsumeAnyTok();
        }
        return true;
      }
    }
    return false;
  }
}

SrcLoc Parser::ConsumeAnyTok(bool consumeCodeCompletionTok) {
  if (IsParenTok()) {
    return ConsumeParen();
  }
  if (IsBracketTok()) {
    return ConsumeBracket();
  }
  if (IsBraceTok()) {
    return ConsumeBrace();
  }
  // if (IsTokenStringLiteral())
  //  return ConsumeStringTok();

  // if (tok.Is(tk::Type::code_completion))
  //  return ConsumeCodeCompletionTok ? ConsumeCodeCompletionToken()
  //                                 : handleUnexpectedCodeCompletionToken();

  // if (Tok.isAnnotation())
  //  return ConsumeAnnotationToken();

  return ConsumeTok();
}

void ParserStats::Print() {}
