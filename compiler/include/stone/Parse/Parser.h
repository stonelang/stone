#ifndef STONE_PARSE_PARSER_H
#define STONE_PARSE_PARSER_H

#include <memory>

#include "stone/Basic/StatisticEngine.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/SyntaxListener.h"
#include "stone/Parse/SyntaxParsing.h"
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
class ParsingDeclarator;

class ParserStats final : public Stats {
  const Parser &parser;

public:
  ParserStats(const Parser &parser)
      : Stats("parser statistics:"), parser(parser) {}
  void Print(ColorfulStream &stream) override;
};

class Parser final {
  friend ParserStats;
  // friend PairDelimiterBalancer;

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
  // Trivia leadingTrivia;

  /// trailing trivias for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  // Trivia trailingTrivia;

  /// Leading trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef leadingTrivia;

  /// Trailing trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef trailingTrivia;

  /// The location of the previous token.
  SrcLoc prevTokLoc;

  // PairDelimiterCount pairDelimiterCount;

  SyntaxScope *curScope;

  // /// Factory object for creating ParsedAttribute objects.
  // AttributeFactory attributeFactory;

  SyntaxParsing syntaxParsing;

private:
  // Identifiers
  // mutable Identifier *importIdentifier;
  // mutable Identifier *moduleIdentifier;

  Parser(SyntaxFile &sf, Syntax &syntax, std::unique_ptr<Lexer> lexer,
         SyntaxListener *listener = nullptr);

public:
  Parser(SyntaxFile &sf, Syntax &syntax, SyntaxListener *listener = nullptr);

  ~Parser();

public:
  ParserStats &GetStats() { return *stats.get(); }
  Lexer &GetLexer() { return *lexer.get(); }
  const Token &GetCurTok() const { return token; }

  void SetSyntaxListener(SyntaxListener *sl) { listener = sl; }
  DeclContext *GetCurDeclContext() { return curDC; }

  // AttributeFactory &GetAttributeFactory() { return attributeFactory; }

  Context &GetContext();

  enum class Notification {
    None,
    DeclCreated,
    StmtCreated,
    ExprCreated,
    TokenConsumed,
  };

public:
  //===--------------------------------------------------------------------===//
  // Decl Parsing

  bool IsStartOfDecl(const Token &tok);
  void ParseTopLevelDecls(llvm::SmallVector<SyntaxResult<Decl>> &results);

  SyntaxResult<Decl> ParseDecl();
  SyntaxResult<Decl> ParseDecl(AccessLevel accessLevel);
  void ParseForwardDecl();

  void ParseInheritance();

private:
  SyntaxResult<Decl> ParseTopLevelDecl();

public:
  // == Type Parsing ==//
  SyntaxResult<QualType> ParseType();
  SyntaxResult<QualType> ParseDeclResultType(Diag<> diagID);
  SyntaxResult<QualType> ParseSimpleType(Diag<> diagID);

public:
  //==Begin Function==//
  SyntaxResult<Decl> ParseFunDecl(AccessLevel accessLevel);
  // void ParseFunForwardDecl(AccessLevel accessLevel);

private:
  SyntaxStatus ParseFunctionSignature(FunDecl &funDecl);
  SyntaxStatus ParseFunctionArguments(FunDecl &funDecl);
  SyntaxStatus ParseFunctionBody(FunDecl &funDecl);

  //==End Function==//
public:
  //=struct=//
  SyntaxResult<Decl> ParseStructDecl();
  void ParseStructForwardDecl();

public:
  bool IsStartOfStmt();

private:
  void Lex(Token &result) { lexer->Lex(result); }

  void Lex(Token &result, llvm::StringRef &leading, llvm::StringRef &trailing) {
    lexer->Lex(result, leading, trailing);
  }

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

public:
  // == Token consumption ==//
  SrcLoc ConsumeToken(
      Parser::Notification notification = Parser::Notification::TokenConsumed);

  SrcLoc ConsumeToken(tok kind) {
    assert(token.Is(kind) && "Consuming wrong token type");
    return ConsumeToken(Parser::Notification::None);
  }
  SrcLoc ConsumeIdentifier(Identifier *result = nullptr);

  /// If the current token is the specified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool ConsumeIf(tok kind) {
    if (token.IsNot(kind)) {
      return false;
    }
    ConsumeToken(kind);
    return true;
  }
  /// If the current token is the specified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool ConsumeIf(tok kind, SrcLoc &consumedLoc) {
    if (token.IsNot(kind)) {
      return false;
    }
    consumedLoc = ConsumeToken(kind);
    return true;
  }
  /// Consume the starting '<' of the current token, which may either
  /// be a complete '<' token or some kind of operator token starting with '<',
  /// e.g., '<>'.
  SrcLoc ConsumeStartingLess();
  /// Consume the starting '>' of the current token, which may either
  /// be a complete '>' token or some kind of operator token starting with '>',
  /// e.g., '>>'.
  SrcLoc ConsumeStartingGreater();
  SrcLoc ConsumeStartingCharOfCurToken(tok Kind = tok::oper_binary_unspaced,
                                       size_t len = 1);

public:
  // == Skipping ==/

  SyntaxStatus SkipUntil(tok T1, tok T2 = tok::MAX);
  void SkipUntilAnyOperator();

  /// Skip until a token that starts with '>', and consume it if found.
  /// Applies heuristics that are suitable when trying to find the end of a list
  /// of generic parameters, generic arguments, or list of types in a protocol
  /// composition.
  SrcLoc SkipUntilGreaterInTypeList(bool interfaceComposition = false);

  /// skipUntilDeclStmtRBrace - Skip to the next decl or '}'.
  void SkipUntilDeclRBrace();

  void SkipUntilDeclStmtRBrace(tok T1);
  void SkipUntilDeclStmtRBrace(tok T1, tok T2);
  void SkipUntilDeclRBrace(tok T1, tok T2);
  void SkipListUntilDeclRBrace(SrcLoc startLoc, tok T1, tok T2);

  /// Skip a single token, but match parentheses, braces, and square brackets.
  ///
  /// Note: this does \em not match angle brackets ("<" and ">")! These are
  /// matched in the source when they refer to a generic type,
  /// but not when used as comparison operators.
  ///
  /// Returns a parser status that can capture whether a code completion token
  /// was returned.
  SyntaxStatus SkipSingle();
  /// Skip until the next '#else', '#endif' or until eof.
  void SkipUntilConditionalBlockClose();

  /// Skip until either finding \c T1 or reaching the end of the line.
  ///
  /// This uses \c skipSingle and so matches parens etc. After calling, one or
  /// more of the following will be true: Tok.is(T1), Tok.isStartOfLine(),
  /// Tok.is(tok::eof). The "or more" case is the first two: if the next line
  /// starts with T1.
  ///
  /// \returns true if there is an instance of \c T1 on the current line (this
  /// avoids the foot-gun of not considering T1 starting the next line for a
  /// plain Tok.is(T1) check).
  bool SkipUntilTokenOrEndOfLine(tok T1, tok T2 = tok::MAX);

  /// Skip a braced block (e.g. function body). The current token must be '{'.
  /// Returns \c true if the parser hit the eof before finding matched '}'.
  ///
  /// Set \c HasNestedTypeDeclarations to true if a token for a type
  /// declaration is detected in the skipped block.
  bool SkipBracedBlock(bool &hasNestedTypeDeclarations);

  /// Skip over any attribute.
  void SkipAnyAttribute();

public:
  // /// EnterScope - start a new scope.
  // void EnterScope(SyntaxScopeKind scopeKind);

  // /// ExitScope - pop a scope off the scope stack.
  // void ExitScope();

  // SyntaxScope *GetCurScope() const;

public:
  InFlightDiagnostic PrintD(SrcLoc loc, Diag<> diagID);
  InFlightDiagnostic PrintD(Token &token, Diag<> diagID);

private:
  // Helpers
  const Token &Peek() const { return lexer->Peek(); }

private:
  //===--------------------------------------------------------------------===//
  // Helpers

  bool IsRightBrace() { return (token.GetKind() == tok::r_brace); }
  bool IsLeftBrace() { return (token.GetKind() == tok::l_brace); }

  bool StartsWithSymbol(Token tok, char symbol) {
    return (tok.IsAnyOperator() || tok.IsPunctuation()) &&
           tok.GetText()[0] == symbol;
  }
  /// Check whether the current token starts with '<'.
  bool StartsWithLess(Token tok) { return StartsWithSymbol(tok, '<'); }
  /// Check whether the current token starts with '>'.
  bool StartsWithGreater(Token tok) { return StartsWithSymbol(tok, '>'); }

public:
  Identifier &GetIdentifierOnly(llvm::StringRef text);
};
/// To assist debugging parser crashes, tell us the location of the
/// current token.
class ParserPrettyStackTrace final : public llvm::PrettyStackTraceEntry {
  Parser &parser;

public:
  explicit ParserPrettyStackTrace(Parser &parser) : parser(parser) {}
  void print(llvm::raw_ostream &out) const override;
};

} // namespace syn
} // namespace stone
#endif
