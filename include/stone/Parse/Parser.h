#ifndef STONE_PARSE_PARSER_H
#define STONE_PARSE_PARSER_H

#include <memory>

#include "stone/Basic/StatisticEngine.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Parse/SyntaxParsing.h"
#include "stone/Parse/SyntaxScopeCache.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxBuilder.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxResult.h"

#include "llvm/Support/Timer.h"

namespace stone {
class SyntaxListener;
namespace syn {

class Syntax;
class Parser;
class SyntaxScope;
class PairDelimiterBalancer;
class ParsingDeclSpecifier;

class ParserStats final : public Stats {
  Parser &parser;

public:
  ParserStats(Parser &parser) : Stats("parser statistics:"), parser(parser) {}
  void Print(ColorfulStream &stream) override;
};

class Parser final {
  friend ParserStats;
  friend PairDelimiterBalancer;

  SyntaxScopeCache scopeCache;
  SyntaxListener *listener;
  std::unique_ptr<Lexer> lexer;
  std::unique_ptr<ParserStats> stats;

  Syntax &syntax;
  SyntaxFile &sf;
  DeclContext *curDC;

  /// This is the current token being considered by the parser.
  Token token;

  /// leading trivias for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  Trivia leadingTrivia;

  /// trailing trivias for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  Trivia trailingTrivia;

  /// The location of the previous token.
  SrcLoc prevTokLoc;

  unsigned short parenCount = 0;
  unsigned short bracketCount = 0;
  unsigned short braceCount = 0;

  SyntaxScope *curScope;

private:
  // Identifiers
  mutable Identifier *importIdentifier;
  mutable Identifier *moduleIdentifier;

public:
  Parser(SyntaxFile &sf, Syntax &syntax, SyntaxListener *listener = nullptr);

  Parser(SyntaxFile &sf, Syntax &syntax, std::unique_ptr<Lexer> lexer,
         SyntaxListener *listener = nullptr);

  ~Parser();

public:
  ParserStats &GetStats() { return *stats.get(); }
  Lexer &GetLexer() { return *lexer.get(); }
  const Token &GetCurTok() const { return token; }

  void SetSyntaxListener(SyntaxListener *sl) { listener = sl; }
  DeclContext *GetCurDeclContext() { return curDC; }

public:
  //===--------------------------------------------------------------------===//
  // Decl Parsing

  bool AtStartOfDecl(const Token &tok);
  void ParseTopLevelDecls(llvm::SmallVector<SyntaxResult<Decl>> &results);

  SyntaxResult<Decl> ParseDecl();
  SyntaxResult<Decl> ParseDecl(AccessLevel accessLevel);
  void ParseForwardDecl();

private:
  SyntaxResult<Decl> ParseTopLevelDecl();

public:
  //=fun=//
  SyntaxResult<Decl> ParseFunDecl(AccessLevel accessLevel);
  void ParseFunForwardDecl(AccessLevel accessLevel);

private:
  SyntaxStatus ParseFunctionSignature(FunDeclSyntaxBuilder &builder);
  SyntaxStatus ParseFunctionArguments(FunDeclSyntaxBuilder &builder);
  SyntaxStatus ParseFunctionBody(FunDeclSyntaxBuilder &builder);

public:
  //=struct=//
  SyntaxResult<Decl> ParseStructDecl();
  void ParseStructForwardDecl();

public:
  // Template
  // SyntaxResult<TemplateDecl *> ParseTemplateDecl(ParsingDeclSpecifier &pds);
private:
  void Lex(Token &result) { lexer->Lex(result); }

  // void Lex(Token &result, Trivia &leading, Trivia &trailing) {
  //   lexer->Lex(result, leading, trailing);
  // }

  /// isTokenParen - Return true if the cur token is '(' or ')'.
  bool IsParenTok() const { return token.IsAny(tok::l_paren, tok::r_paren); }
  /// isTokenBracket - Return true if the cur token is '[' or ']'.
  bool IsBracketTok() const {
    return token.IsAny(tok::l_square, tok::r_square);
  }
  /// isTokenBrace - Return true if the cur token is '{' or '}'.
  bool IsBraceTok() const { return token.IsAny(tok::l_brace, tok::r_brace); }
  /// isTokenStringLiteral - True if this token is a string-literal.
  // bool IsTokStringLiteral() const {
  //  return tok::isStringLiteral(Tok.getKind());
  //}

public:
  /// Control flags for SkipUntil functions.
  enum SkipToFlags {
    ///< Stop skipping at semicolon
    StopAtSemi = 1 << 0,
    /// Stop skipping at specified token, but don't skip the token itself
    StopBeforeMatch = 1 << 1,
    ///< Stop at code completion
    StopAtCodeCompletion = 1 << 2
  };

  friend constexpr SkipToFlags operator|(SkipToFlags L, SkipToFlags R) {
    return static_cast<SkipToFlags>(static_cast<unsigned>(L) |
                                    static_cast<unsigned>(R));
  }

  /// SkipUntil - Read tokens until we get to the specified token, then consume
  /// it (unless StopBeforeMatch is specified).  Because we cannot guarantee
  /// that the token will ever occur, this skips to the next token, or to some
  /// likely good stopping point.  If Flags has StopAtSemi flag, skipping will
  /// stop at a ';' character.
  ///
  /// If SkipTo finds the specified token, it returns true, otherwise it
  /// returns false.
  bool SkipTo(tok ty, SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    return SkipTo(llvm::makeArrayRef(ty), flags);
  }
  bool SkipTo(tok ty1, tok ty2,
              SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    tok tokArray[] = {ty1, ty2};
    return SkipTo(tokArray, flags);
  }
  bool SkipTo(tok ty1, tok ty2, tok ty3,
              SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    tok tokArray[] = {ty1, ty2, ty3};
    return SkipTo(tokArray, flags);
  }
  bool SkipTo(llvm::ArrayRef<tok> toks,
              SkipToFlags flags = static_cast<SkipToFlags>(0));

public:
  // First, call ParseFunDecl -- this is your fun prototype
  // Then you call the following:
  // void ParseFunDeclDefinition();

  SyntaxResult<Decl> ParseSpaceDecl();
  ///
public:
  /// Stmt
  SyntaxResult<Stmt> ParseStmt();

public:
  /// Expr
  SyntaxResult<Expr> ParseExpr();

public:
  /// Stop parsing now.
  void Stop() { token.SetKind(tok::eof); }

  /// Is at end of file.
  bool IsDone() { return token.GetKind() == tok::eof; }

  bool HasError();

  SrcLoc ConsumeBracket() { return SrcLoc(); }
  SrcLoc ConsumeBrace() { return SrcLoc(); }

  SrcLoc ConsumeParen() {
    assert(IsParenTok() && "Wrong consume method");
    if (token.GetKind() == tok::l_paren)
      ++parenCount;
    else if (parenCount) {
      // TODO: angleBrackets.clear(*this);
      --parenCount; // Don't let unbalanced )'s drive the count negative.
    }
    prevTokLoc = token.GetLoc();
    Lex(token);
    return prevTokLoc;
  }

  /// Consume the token and update OnToken from SCPipeline
  SrcLoc ConsumeTok(bool onTok = true);
  SrcLoc ConsumeTok(tok kind) {
    assert(token.Is(kind) && "Consuming wrong token type");
    return ConsumeTok(false);
  }
  SrcLoc ConsumeAnyTok(bool consumeCodeCompletionTok = false);
  SrcLoc ConsumeIdentifier(Identifier *result = nullptr);
  Context &GetContext();

public:
  /// EnterScope - start a new scope.
  void EnterScope(SyntaxScopeKind scopeKind);

  /// ExitScope - pop a scope off the scope stack.
  void ExitScope();

  SyntaxScope *GetCurScope() const;

public:
  InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID);
  InFlightDiagnostic PrintD(const Token &token, DiagID diagID);

private:
  void PrintD();

private:
  // Helpers
  const Token &Peek() const { return lexer->Peek(); }

private:
  //===--------------------------------------------------------------------===//
  // Helpers

  bool IsRightBrace() { return (token.GetKind() == tok::r_brace); }
  bool IsLeftBrace() { return (token.GetKind() == tok::l_brace); }
};

/// To assist debugging parser crashes, tell us the location of the
/// current token.
class ParserPrettyStackTrace : public llvm::PrettyStackTraceEntry {
  Parser &parser;

public:
  explicit ParserPrettyStackTrace(Parser &parser) : parser(parser) {}
  void print(llvm::raw_ostream &out) const override;
};

} // namespace syn
} // namespace stone
#endif
