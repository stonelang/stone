#ifndef STONE_PARSE_PARSER_H
#define STONE_PARSE_PARSER_H

#include "stone/Basic/StatisticEngine.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/SyntaxScopeCache.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxResult.h"
#include "stone/Syntax/TreeContext.h"

#include "llvm/Support/Timer.h"

#include <memory>

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
  ParserStats(Parser &parser, Basic &basic)
      : Stats("parser statistics:", basic), parser(parser) {}
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
  class ParsingScope final {
    Parser *self;
    ParsingScope(const ParsingScope &) = delete;
    void operator=(const ParsingScope &) = delete;

  public:
    // ParseScope - Construct a new object to manage a scope in the
    // parser Self where the new Scope is created with the flags
    // ScopeFlags, but only when we aren't about to enter a compound statement.
    ParsingScope(Parser *self, unsigned scopeFlags, bool enteredScope = true,
                 bool beforeCompoundStmt = false)
        : self(self) {

      if (enteredScope && !beforeCompoundStmt) {
        self->EnterScope(scopeFlags);
      } else {
        if (beforeCompoundStmt) {
          // TODO: self->incrementMSManglingNumber();
        }
        this->self = nullptr;
      }
    }
    // Exit - Exit the scope associated with this object now, rather
    // than waiting until the object is destroyed.
    void Exit() {
      if (self) {
        self->ExitScope();
        self = nullptr;
      }
    }
    ~ParsingScope() { Exit(); }
  };

public:
  class MultiParsingScope final {
  public:
  };

public:
  ParserStats &GetStats() { return *stats.get(); }
  Lexer &GetLexer() { return *lexer.get(); }
  const Token &GetCurTok() const { return tok; }

  void SetSyntaxListener(SyntaxListener *sl) { listener = sl; }
  DeclContext *GetCurDeclContext() { return curDC; }

public:
  bool IsTopDeclStart(const Token &tok);
  bool ParseTopDecl(DeclGroupPtrTy &result, bool isFirstDecl = false);
  syn::DeclGroupPtrTy ParseDecl(ParsingDeclSpecifier *pds = nullptr);

  syn::DeclGroupPtrTy ParseDecl(ParsingDeclSpecifier &pds,
                                AccessLevel al = AccessLevel::None);

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
  void Init();
  void Lex(Token &result) { lexer->Lex(result); }
  void Lex(Token &result, Trivia &leading, Trivia &trailing) {
    lexer->Lex(result, leading, trailing);
  }

  /// isTokenParen - Return true if the cur token is '(' or ')'.
  bool IsParenTok() const {
    return tok.IsAny(tk::Type::l_paren, tk::Type::r_paren);
  }
  /// isTokenBracket - Return true if the cur token is '[' or ']'.
  bool IsBracketTok() const {
    return tok.IsAny(tk::Type::l_square, tk::Type::r_square);
  }
  /// isTokenBrace - Return true if the cur token is '{' or '}'.
  bool IsBraceTok() const {
    return tok.IsAny(tk::Type::l_brace, tk::Type::r_brace);
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
  bool SkipTo(tk::Type ty, SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    return SkipTo(llvm::makeArrayRef(ty), flags);
  }
  bool SkipTo(tk::Type ty1, tk::Type ty2,
              SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    tk::Type tokArray[] = {ty1, ty2};
    return SkipTo(tokArray, flags);
  }
  bool SkipTo(tk::Type ty1, tk::Type ty2, tk::Type ty3,
              SkipToFlags flags = static_cast<SkipToFlags>(0)) {
    tk::Type tokArray[] = {ty1, ty2, ty3};
    return SkipTo(tokArray, flags);
  }
  bool SkipTo(llvm::ArrayRef<tk::Type> toks,
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
  void Stop() { tok.SetType(tk::Type::eof); }

  /// Is at end of file.
  bool IsDone() { return tok.GetType() == tk::Type::eof; }

  bool HasError();

  SrcLoc ConsumeBracket() { return SrcLoc(); }
  SrcLoc ConsumeBrace() { return SrcLoc(); }

  SrcLoc ConsumeParen() {
    assert(IsParenTok() && "Wrong consume method");
    if (tok.GetType() == tk::Type::l_paren)
      ++parenCount;
    else if (parenCount) {
      // TODO: angleBrackets.clear(*this);
      --parenCount; // Don't let unbalanced )'s drive the count negative.
    }
    prevTokLoc = tok.GetLoc();
    Lex(tok);
    return prevTokLoc;
  }

  /// Consume the token and update OnToken from CompilerPipeline
  SrcLoc ConsumeTok(bool onTok = true);
  SrcLoc ConsumeTok(tk::Type ty) {
    assert(tok.Is(ty) && "Consuming wrong token type");
    return ConsumeTok(false);
  }
  SrcLoc ConsumeAnyTok(bool consumeCodeCompletionTok = false);
  SrcLoc ConsumeIdentifier(Identifier *result = nullptr);
  Basic &GetBasic();

public:
  /// EnterScope - start a new scope.
  void EnterScope(unsigned scopeFlags);

  /// ExitScope - pop a scope off the scope stack.
  void ExitScope();

  SyntaxScope *GetCurScope() const;

public:
  InFlightDiagnostic Diagnose(SrcLoc loc, DiagID diagID);
  InFlightDiagnostic Diagnose(const Token &tok, DiagID diagID);

  // InFlightDiagnostic Diagnose(unsigned DiagID) {
  //   return Diag(tok, diagID);
  // }

private:
  void Diagnose();
};

} // namespace syn
} // namespace stone
#endif
