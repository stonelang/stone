#include "stone/Compile/Parser.h"

#include "stone/Core/Context.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/SyntaxDiagnostic.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxScope.h"

using namespace stone;
using namespace stone::syn;

Parser::Parser(SyntaxFile &sf, Syntax &syntax, SyntaxListener *listener)
    : Parser(sf, syntax,
             std::unique_ptr<Lexer>(new Lexer(
                 sf.GetSrcID(), syntax.GetSyntaxContext().GetSrcMgr(),
                 &syntax.GetSyntaxContext().GetContext().GetDiagEngine(),
                 &syntax.GetSyntaxContext().GetContext().GetStatEngine())),
             listener) {}

Parser::Parser(SyntaxFile &sf, Syntax &syntax, std::unique_ptr<Lexer> lx,
               SyntaxListener *listener)
    : sf(sf), syntax(syntax), lexer(lx.release()), curDC(&sf),
      listener(listener) {

  stats.reset(new ParserStats(*this));
  GetContext().GetStatEngine().Register(stats.get());

  Initialize();
}
void Parser::Initialize() {
  // TODO: fishy?
  if (Peek().Is(tok::MAX) && (token.Is(tok::MAX))) {
    // This is an empty file
    Stop();
    return;
  }
  // Create the translation unit scope.  Install it as the current scope.
  // assert(GetCurScope() == nullptr && "A scope is already active?");

  // Prime the Parser's token
  // The Lexer has the first token but the Parser's token defaults to tk::MAX
  // So, update the parser's token with the first token from the Lexer
  if (token.Is(tok::MAX)) {
    ConsumeTok();
  }
}

Parser::~Parser() {}

SyntaxScope *Parser::GetCurScope() const {
  assert(false && "Not implemented");
  return nullptr;
}

bool Parser::HasError() { return GetContext().HasError(); }
Context &Parser::GetContext() { return syntax.GetSyntaxContext().GetContext(); }

void Parser::EnterScope(SyntaxScopeKind scopeKind) {}
void Parser::ExitScope() {}

SrcLoc Parser::ConsumeTok(bool onTok) {
  SrcLoc loc = token.GetLoc();
  assert(token.IsNot(tok::eof) && "Lexing past eof!");
  Lex(token, leadingTrivia, trailingTrivia);
  prevTokLoc = loc;
  return loc;
}

static bool HasFlagsSet(Parser::SkipToFlags L, Parser::SkipToFlags R) {
  return (static_cast<unsigned>(L) & static_cast<unsigned>(R)) != 0;
}
bool Parser::SkipTo(llvm::ArrayRef<tok> toks, SkipToFlags flags) {
  // We always want this function to skip at least one token if the first token
  // isn't T and if not at EOF.
  bool isFirstTokenSkipped = true;
  while (true) {
    // If we found one of the tokens, stop and return true.
    for (unsigned i = 0, numToks = toks.size(); i != numToks; ++i) {
      if (token.Is(toks[i])) {
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

  // if (token.Is(tok::code_completion))
  //  return ConsumeCodeCompletionTok ? ConsumeCodeCompletionToken()
  //                                 : handleUnexpectedCodeCompletionToken();

  // if (Tok.isAnnotation())
  //  return ConsumeAnnotationToken();

  return ConsumeTok();
}

void ParserStats::Print(ColorfulStream &stream) {}
