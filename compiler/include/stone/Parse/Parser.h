#ifndef STONE_PARSE_PARSER_H
#define STONE_PARSE_PARSER_H

#include <memory>

#include "stone/Basic/StableHasher.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/Parsing.h"
#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/SyntaxOptions.h"
#include "stone/Syntax/SyntaxResult.h"

#include "llvm/Support/Timer.h"

namespace stone {
class SyntaxListener;
namespace syn {

class BraceStmt;
class Syntax;
class Parser;
class Scope;
class PairDelimiterBalancer;
class ParsingDeclCollector;
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
  friend ParsingToken;

  // friend PairDelimiterBalancer;

  SyntaxListener *listener;
  std::unique_ptr<Lexer> lexer;
  std::unique_ptr<ParserStats> stats;

  SyntaxContext &sc;
  SyntaxFile &sf;
  DeclContext *curDC;

  /// This is the current curTok being considered by the parser.
  Token curTok;

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

  /// The location of the previous tok.
  SrcLoc prevTokLoc;

  // PairDelimiterCount pairDelimiterCount;

  ParsingToken parsingTok;

  ScopeCache scopeCache;

  // UInt16 ParenCount = 0;
  // UInt16 BraceCount = 0;
  // UInt16 BrackeCount = 0;

private:
  // Identifiers
  // mutable Identifier *importIdentifier;
  // mutable Identifier *moduleIdentifier;

  Parser(SyntaxFile &sf, SyntaxContext &sc, std::unique_ptr<Lexer> lexer,
         SyntaxListener *listener = nullptr);

public:
  Parser(SyntaxFile &sf, SyntaxContext &sc, SyntaxListener *listener = nullptr);

  ~Parser();

public:
  ParserStats &GetStats() { return *stats; }
  Lexer &GetLexer() { return *lexer; }
  const Token &GetTok() const { return curTok; }
  SyntaxContext &GetSyntaxContext() { return sc; }

  void SetSyntaxListener(SyntaxListener *sl) { listener = sl; }
  DeclContext *GetCurDeclContext() { return curDC; }

  LangContext &GetLangContext() { return sc.GetLangContext(); }

  /// The current curTok hash, or \c None if the parser isn't computing a hash
  /// for the curTok stream.
  llvm::Optional<StableHasher> currentTokenHash;

public:
  void RecordTokenHash(const Token Tok) {
    if (!Tok.GetText().empty())
      RecordTokenHash(Tok.GetText());
  }
  void RecordTokenHash(StringRef curTok);

public:
  void ParseTopLevelDecls(llvm::SmallVector<SyntaxResult<Decl>> &results);

private:
  SyntaxResult<Decl> ParseTopLevelDecl();

public:
  // TODO: We only need on ParseDecl
  SyntaxResult<Decl> ParseDecl(ParsingDeclOptions flags,
                               ParsingDeclCollector *collector = nullptr);

  void ParseDeclName();

  // SyntaxStatus CollectDecl(ParsingDeclCollector &collector);

private:
  SyntaxResult<Decl> ParseDeclInternal(ParsingDeclCollector &collector);

public:
  // TODO: Param should be constant
  SyntaxResult<Decl> ParseVarDecl(ParsingDeclCollector &collector);
  SyntaxResult<Decl> ParseAutoDecl(ParsingDeclCollector &collector);

public:
  // === Collectors === ///
  SyntaxStatus CollectDecl(ParsingDeclCollector &collector);
  SyntaxStatus CollectUsingDecl(ParsingDeclCollector &collector);
  SyntaxStatus CollectAccessLevel(ParsingDeclCollector &collector);

  bool IsTypeThunk(const Token &tk);
  SyntaxStatus CollectTypeThunk(TypeCollector &collector);
  SyntaxStatus CollectTypeThunks(TypeCollector &collector);
  SyntaxStatus CollectBasicTypeDecl(TypeCollector &collector);
  SyntaxStatus CollectNominalTypeDecl(TypeCollector &collector);
  SyntaxStatus CollectTypeQualifiers(TypeCollector &collector);
  SyntaxStatus CollectTypeQualifier(TypeCollector &collector);
  SyntaxStatus CollectTypeOperator(TypeCollector &collector);
  SyntaxStatus CollectStorageSpecifier(ParsingDeclCollector &collector);
  SyntaxStatus CollectFunctionDecl(ParsingDeclCollector &collector);
  SyntaxStatus VerifyDeclCollected(ParsingDeclCollector &collector);

public:
  // === Type Parsing ===//

  bool IsBasicType(tok kind) const;

  // TODO: Passing ParsingDeclCollector -- may just want to pass the Type
  // collectors in the furture. This is ok for now.
  Type ParseType(TypeCollector &collector, Diag<> diagID);
  Type ParseFunctionType(TypeCollector &collector, Diag<> diagID);
  Type ParsePointerType(TypeCollector &collector, Diag<> diagID);
  Type ParseReferenceType(TypeCollector &collector, Diag<> diagID);
  Type ParseDeclResultType(TypeCollector &collector, Diag<> diagID);
  Type ParseBasicType(TypeCollector &collector, Diag<> diagID);
  Type ParseIdentifierType(TypeCollector &collector, Diag<> diagID);

public:
  //== fun ==//
  SyntaxResult<Decl> ParseFunDecl(ParsingDeclCollector &collector);

private:
  SyntaxStatus ParseFunctionSignature(ParsingDeclCollector &collector,
                                      Identifier basicName, DeclName &fullName);

  // Identifier functionName,
  //                                       DeclName &fullName,
  //                                       ParameterList *&bodyParams,
  //                                       DefaultArgumentInfo &defaultArgs,
  //                                       SourceLoc &asyncLoc,
  //                                       bool &reasync,
  //                                       SourceLoc &throws,
  //                                       bool &rethrows,
  //                                       Typer *&retType);

  SyntaxStatus ParseFunctionArguments(ParsingDeclCollector &collectorifier);
  SyntaxStatus ParseFunctionBody(ParsingDeclCollector &collectorifier,
                                 FunctionDecl &functionDecl);

  BraceStmt *ParseFunctionBodyImpl(ParsingDeclCollector &collectorifier,
                                   FunctionDecl &funDecl);

public:
  //== using ==//
  SyntaxResult<Decl> ParseUsingDecl(ParsingDeclCollector &collectorifier);

public:
  //== struct ==//
  SyntaxResult<Decl> ParseStructDecl(ParsingDeclCollector &collectorifier);

public:
  //== enum== //
  SyntaxResult<Decl> ParseEnumDecl(ParsingDeclCollector &collectorifier);

public:
  //== interface ==//
  SyntaxResult<Decl> ParseInterfaceDecl(ParsingDeclCollector &collectorifier);

private:
  void Lex(Token &result) { lexer->Lex(result); }
  void Lex(Token &result, llvm::StringRef &leading, llvm::StringRef &trailing) {
    lexer->Lex(result, leading, trailing);
  }

public:
  SyntaxResult<Decl> ParseSpaceDecl();

public:
  bool IsStartOfStmt();
  /// Stmt
  SyntaxResult<Stmt> ParseStmt();

public:
  /// Expr
  SyntaxResult<Expr> ParseExpr();

public:
  /// Stop parsing now.
  void EndParsing() { curTok.SetKind(tok::eof); }
  /// Is at end of file.
  bool IsEOF() { return curTok.GetKind() == tok::eof; }
  bool IsParsing() { return (!IsEOF() && !HasError()); }
  bool HasError() { return GetLangContext().GetDiagUnit().HasError(); }
  DiagnosticEngine &GetDiags() {
    return GetLangContext().GetDiagUnit().GetDiagEngine();
  }

public:
  //===--------------------------------------------------------------------===//
  // Routines to save and restore parser state.

  ParsingPosition GetParsingPosition() {
    return ParsingPosition(
        GetLexer().getStateForBeginningOfToken(curTok, leadingTrivia),
        prevTokLoc);
  }
  ParsingPosition GetParsingPosition(SrcLoc loc, SrcLoc previousLoc) {
    return ParsingPosition(GetLexer().getStateForBeginningOfTokenLoc(loc),
                           previousLoc);
  }
  void RestoreParsingPosition(ParsingPosition parsingPos,
                              bool enableDiagnostics = false) {
    GetLexer().restoreState(parsingPos.lexingState, enableDiagnostics);
    Lex(curTok, leadingTrivia, trailingTrivia);
    prevTokLoc = parsingPos.prevLoc;
  }

  void BackTrackParsingPosition(ParsingPosition parsingPos) {
    assert(parsingPos.isValid());
    GetLexer().backtrackToState(parsingPos.lexingState);
    Lex(curTok, leadingTrivia, trailingTrivia);
    prevTokLoc = parsingPos.prevLoc;
  }

public:
  // == Token consumption ==//
  SrcLoc ConsumeToken(
      ParsingNotification notification = ParsingNotification::TokenConsumed);

  SrcLoc ConsumeToken(tok kind) {
    assert(curTok.Is(kind) && "Consuming wrong curTok type");
    return ConsumeToken(ParsingNotification::None);
  }

  /// If the current curTok is the collectorified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool ConsumeIf(tok kind) {
    if (curTok.IsNot(kind)) {
      return false;
    }
    ConsumeToken(kind);
    return true;
  }
  /// If the current curTok is the collectorified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool ConsumeIf(tok kind, SrcLoc &consumedLoc) {
    if (curTok.IsNot(kind)) {
      return false;
    }
    consumedLoc = ConsumeToken(kind);
    return true;
  }
  /// Consume the starting '<' of the current curTok, which may either
  /// be a complete '<' curTok or some kind of operator curTok starting with
  /// '<', e.g., '<>'.
  SrcLoc ConsumeStartingLess();
  /// Consume the starting '>' of the current curTok, which may either
  /// be a complete '>' curTok or some kind of operator curTok starting with
  /// '>', e.g., '>>'.
  SrcLoc ConsumeStartingGreater();
  SrcLoc ConsumeStartingCharOfCurToken(tok Kind = tok::oper_binary_unspaced,
                                       size_t len = 1);

  SyntaxStatus ParseIdentifier(Identifier &result, SrcLoc &resultLoc);
  SrcLoc ConsumeIdentifier(Identifier &result);

public:
  // == Skipping ==/

  SyntaxStatus SkipUntil(tok T1, tok T2 = tok::MAX);
  void SkipUntilAnyOperator();

  /// Skip until a curTok that starts with '>', and consume it if found.
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

  /// Skip a single curTok, but match parentheses, braces, and square brackets.
  ///
  /// Note: this does \em not match angle brackets ("<" and ">")! These are
  /// matched in the source when they refer to a generic type,
  /// but not when used as comparison operators.
  ///
  /// Returns a parser status that can capture whether a code completion curTok
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

  /// Skip a braced block (e.g. function body). The current curTok must be '{'.
  /// Returns \c true if the parser hit the eof before finding matched '}'.
  ///
  /// Set \c HasNestedTypeDeclarations to true if a curTok for a type
  /// declaration is detected in the skipped block.
  bool SkipBracedBlock(bool &hasNestedTypeDeclarations);

  /// Skip over any attribute.
  void SkipAnyAttribute();

public:
  /// EnterScope - start a new scope.
  void EnterScope(ScopeKind scopeKind);

  /// ExitScope - pop a scope off the scope stack.
  void ExitScope();

  Scope *CreateScope(ScopeKind kind, Scope *parent);

  Scope *GetCurScope() const {
    if (HasCurScope()) {
      return scopeCache.back();
    }
    return nullptr;
  }
  bool HasCurScope() const { return (scopeCache.size() > 0); }
  void PopCurScope() { scopeCache.pop_back(); }
  void PushCurScope(Scope *scope) { scopeCache.push_back(scope); }

public:
  InFlightDiagnostic PrintD(SrcLoc loc, Diag<> diagID);
  InFlightDiagnostic PrintD(Token &curTok, Diag<> diagID);

private:
  static Scope *CreateScope(ScopeKind kind, SyntaxContext &sc,
                            DiagnosticEngine &diags, Scope *parent = nullptr);
  // Helpers
  const Token &PeekNextToken() const { return lexer->Peek(); }
  SrcLoc GetLoc() { return curTok.GetLoc(); }

private:
  //===--------------------------------------------------------------------===//
  // Helpers

  bool IsRightBrace() { return (curTok.GetKind() == tok::r_brace); }
  bool IsLeftBrace() { return (curTok.GetKind() == tok::l_brace); }

  bool StartsWithSymbol(Token tok, char symbol) {
    return (tok.IsAnyOperator() || tok.IsPunctuation()) &&
           tok.GetText()[0] == symbol;
  }
  /// Check whether the current curTok starts with '<'.
  bool StartsWithLess(Token tok) { return StartsWithSymbol(tok, '<'); }
  /// Check whether the current curTok starts with '>'.
  bool StartsWithGreater(Token tok) { return StartsWithSymbol(tok, '>'); }

public:
  Identifier GetIdentifier(llvm::StringRef text);
};

} // namespace syn
} // namespace stone
#endif
