#ifndef STONE_PARSE_PARSER_H
#define STONE_PARSE_PARSER_H

#include <memory>

#include "stone/Compile/Lexer.h"
#include "stone/Compile/SyntaxParsing.h"
#include "stone/Compile/SyntaxListener.h"
#include "stone/Compile/SyntaxScopeCache.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Specifier.h"
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
  ParserStats(Parser &parser, Context &ctx)
      : Stats("parser statistics:", ctx), parser(parser) {}
  void Print() override;
};

class Parser final {
  friend ParserStats;
  friend PairDelimiterBalancer;

  Syntax &syntax;
  SyntaxFile &sf;

  SyntaxScopeCache scopeCache;
  SyntaxListener *listener;
  std::unique_ptr<Lexer> lexer;
  std::unique_ptr<ParserStats> stats;

  DeclContext *curDC;

  /// This is the current token being considered by the parser.
  Token tok;

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

  void Initialize();

public:
  ParserStats &GetStats() { return *stats.get(); }
  Lexer &GetLexer() { return *lexer.get(); }
  const Token &GetCurTok() const { return tok; }

  void SetSyntaxListener(SyntaxListener *sl) { listener = sl; }
  DeclContext *GetCurDeclContext() { return curDC; }

public:
  //===--------------------------------------------------------------------===//
  // Decl Parsing

  bool AtStartOfDecl(const Token &tok);

  void ParseTopLevelDecl(SyntaxResult<Decl *> &result);
  void ParseTopLevelDecls(llvm::SmallVector<SyntaxResult<Decl *>> &results);

  SyntaxResult<Decl *> ParseDecl(ParsingDeclSpecifier *pds = nullptr);
  SyntaxResult<Decl *> ParseDecl(ParsingDeclSpecifier &pds,
                                 AccessLevel al = AccessLevel::Private);

public:
  // Function
  SyntaxResult<Decl *> ParseFunDecl(ParsingDeclSpecifier &pds,
                                    AccessLevel accessLevel);
  // Function
private:
  void ParseFunctionSignature(FunDecl *funDecl);
  void ParseFunctionArguments(FunDecl *funDecl);
  void ParseFunctionBody(FunDecl *funDecl);

public:
  // Struct
  SyntaxResult<Decl *> ParseStructDecl(ParsingDeclSpecifier &pds);

public:
  // Template
  // SyntaxResult<TemplateDecl *> ParseTemplateDecl(ParsingDeclSpecifier &pds);
private:
  void Lex(Token &result) { lexer->Lex(result); }
  void Lex(Token &result, Trivia &leading, Trivia &trailing) {
    lexer->Lex(result, leading, trailing);
  }

  /// isTokenParen - Return true if the cur token is '(' or ')'.
  bool IsParenTok() const {
    return tok.IsAny(tk::Kind::l_paren, tk::Kind::r_paren);
  }
  /// isTokenBracket - Return true if the cur token is '[' or ']'.
  bool IsBracketTok() const {
    return tok.IsAny(tk::Kind::l_square, tk::Kind::r_square);
  }
  /// isTokenBrace - Return true if the cur token is '{' or '}'.
  bool IsBraceTok() const {
    return tok.IsAny(tk::Kind::l_brace, tk::Kind::r_brace);
  }
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
  bool SkipTo(tk::Kind ty, SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    return SkipTo(llvm::makeArrayRef(ty), flags);
  }
  bool SkipTo(tk::Kind ty1, tk::Kind ty2,
              SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    tk::Kind tokArray[] = {ty1, ty2};
    return SkipTo(tokArray, flags);
  }
  bool SkipTo(tk::Kind ty1, tk::Kind ty2, tk::Kind ty3,
              SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    tk::Kind tokArray[] = {ty1, ty2, ty3};
    return SkipTo(tokArray, flags);
  }
  bool SkipTo(llvm::ArrayRef<tk::Kind> toks,
              SkipToFlags flags = static_cast<SkipToFlags>(0));

public:
  // First, call ParseFunDecl -- this is your fun prototype
  // Then you call the following:
  // void ParseFunDeclDefinition();

  DeclResult ParseSpaceDecl();
  ///
public:
  /// Stmt
  StmtResult ParseStmt();

public:
  /// Expr
  ExprResult ParseExpr();

public:
  /// Stop parsing now.
  void Stop() { tok.SetKind(tk::Kind::eof); }

  /// Is at end of file.
  bool IsDone() { return tok.GetKind() == tk::Kind::eof; }

  bool HasError();

  SrcLoc ConsumeBracket() { return SrcLoc(); }
  SrcLoc ConsumeBrace() { return SrcLoc(); }

  SrcLoc ConsumeParen() {
    assert(IsParenTok() && "Wrong consume method");
    if (tok.GetKind() == tk::Kind::l_paren)
      ++parenCount;
    else if (parenCount) {
      // TODO: angleBrackets.clear(*this);
      --parenCount; // Don't let unbalanced )'s drive the count negative.
    }
    prevTokLoc = tok.GetLoc();
    Lex(tok);
    return prevTokLoc;
  }

  /// Consume the token and update OnToken from SCPipeline
  SrcLoc ConsumeTok(bool onTok = true);
  SrcLoc ConsumeTok(tk::Kind ty) {
    assert(tok.Is(ty) && "Consuming wrong token type");
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
  InFlightDiagnostic PrintD(const Token &tok, DiagID diagID);

  // InFlightDiagnostic PrintD(unsigned DiagID) {
  //   return Diag(tok, diagID);
  // }

private:
  void PrintD();

private:
  // Helpers
  Token &Peek() { return lexer->Peek(); }

private:
  //===--------------------------------------------------------------------===//
  // Helpers

  bool IsRightBrace() { return (tok.GetKind() == tk::Kind::r_brace); }
  bool IsLeftBrace() { return (tok.GetKind() == tk::Kind::l_brace); }
};

} // namespace syn
} // namespace stone
#endif
