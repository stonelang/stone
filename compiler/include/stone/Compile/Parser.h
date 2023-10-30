#ifndef STONE_PARSE_PARSER_H
#define STONE_PARSE_PARSER_H

#include <memory>

#include "stone/Basic/StableHasher.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/Parsing.h"
#include "stone/AST/Attribute.h"
#include "stone/AST/Expr.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/Module.h"
#include "stone/AST/Specifier.h"
#include "stone/AST/Stmt.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/ASTNode.h"
#include "stone/AST/ASTOptions.h"
#include "stone/AST/ASTResult.h"

#include "llvm/Support/Timer.h"

namespace stone {
class ASTListener;
namespace syn {

class BraceStmt;
class AST;
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
  void Print(ColorStream &stream) override;
};

class Parser final {
  friend ParserStats;
  friend ParsingToken;

  // friend PairDelimiterBalancer;

  ASTListener *listener;
  std::unique_ptr<Lexer> lexer;
  std::unique_ptr<ParserStats> stats;

  ASTContext &sc;
  ASTFile &sf;
  DeclContext *curDC;

  /// This is the current curTok being considered by the parser.
  Token curTok;

  /// leading trivias for \c Tok.
  /// Always empty if !SF.shouldBuildASTTree().
  // Trivia leadingTrivia;

  /// trailing trivias for \c Tok.
  /// Always empty if !SF.shouldBuildASTTree().
  // Trivia trailingTrivia;

  /// Leading trivia for \c Tok.
  /// Always empty if !SF.shouldBuildASTTree().
  llvm::StringRef leadingTrivia;

  /// Trailing trivia for \c Tok.
  /// Always empty if !SF.shouldBuildASTTree().
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

  Parser(ASTFile &sf, ASTContext &sc, std::unique_ptr<Lexer> lexer,
         ASTListener *listener = nullptr);

public:
  Parser(ASTFile &sf, ASTContext &sc, ASTListener *listener = nullptr);

  ~Parser();

public:
  ParserStats &GetStats() { return *stats; }
  Lexer &GetLexer() { return *lexer; }
  const Token &GetTok() const { return curTok; }
  ASTContext &GetASTContext() { return sc; }

  void SetASTListener(ASTListener *sl) { listener = sl; }
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
  void ParseTopLevelDecls(llvm::SmallVector<ASTResult<Decl>> &results);

private:
  ASTResult<Decl> ParseTopLevelDecl();

public:
  // TODO: We only need on ParseDecl
  ASTResult<Decl> ParseDecl(ParsingDeclOptions flags,
                               ParsingDeclCollector *collector = nullptr);

  void ParseDeclName();

  // ASTStatus CollectDecl(ParsingDeclCollector &collector);

private:
  ASTResult<Decl> ParseDeclInternal(ParsingDeclCollector &collector);

public:
  // TODO: Param should be constant
  ASTResult<Decl> ParseVarDecl(ParsingDeclCollector &collector);
  ASTResult<Decl> ParseAutoDecl(ParsingDeclCollector &collector);

public:
  // === Collectors === ///
  ASTStatus CollectDecl(ParsingDeclCollector &collector);
  ASTStatus CollectUsingDecl(ParsingDeclCollector &collector);
  ASTStatus CollectAccessLevel(ParsingDeclCollector &collector);

  bool IsTypeThunk(const Token &tk);
  ASTStatus CollectTypeThunk(TypeCollector &collector);
  ASTStatus CollectTypeThunks(TypeCollector &collector);
  ASTStatus CollectBasicTypeDecl(TypeCollector &collector);
  ASTStatus CollectNominalTypeDecl(TypeCollector &collector);
  ASTStatus CollectTypeQualifiers(TypeCollector &collector);
  ASTStatus CollectTypeQualifier(TypeCollector &collector);
  ASTStatus CollectTypeOperator(TypeCollector &collector);
  ASTStatus CollectStorageSpecifier(ParsingDeclCollector &collector);
  ASTStatus CollectFunctionDecl(ParsingDeclCollector &collector);
  ASTStatus VerifyDeclCollected(ParsingDeclCollector &collector);

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
  ASTResult<Decl> ParseFunDecl(ParsingDeclCollector &collector);

private:
  ASTStatus ParseFunctionSignature(ParsingDeclCollector &collector,
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

  ASTStatus ParseFunctionArguments(ParsingDeclCollector &collectorifier);
  ASTStatus ParseFunctionBody(ParsingDeclCollector &collectorifier,
                                 FunctionDecl &functionDecl);

  BraceStmt *ParseFunctionBodyImpl(ParsingDeclCollector &collectorifier,
                                   FunctionDecl &funDecl);

public:
  //== using ==//
  ASTResult<Decl> ParseUsingDecl(ParsingDeclCollector &collectorifier);

public:
  //== struct ==//
  ASTResult<Decl> ParseStructDecl(ParsingDeclCollector &collectorifier);

public:
  //== enum== //
  ASTResult<Decl> ParseEnumDecl(ParsingDeclCollector &collectorifier);

public:
  //== interface ==//
  ASTResult<Decl> ParseInterfaceDecl(ParsingDeclCollector &collectorifier);

private:
  void Lex(Token &result) { lexer->Lex(result); }
  void Lex(Token &result, llvm::StringRef &leading, llvm::StringRef &trailing) {
    lexer->Lex(result, leading, trailing);
  }

public:
  ASTResult<Decl> ParseSpaceDecl();

public:
  bool IsStartOfStmt();
  /// Stmt
  ASTResult<Stmt> ParseStmt();

public:
  /// Expr
  ASTResult<Expr> ParseExpr();

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

  ASTStatus ParseIdentifier(Identifier &result, SrcLoc &resultLoc);
  SrcLoc ConsumeIdentifier(Identifier &result);

public:
  // == Skipping ==/

  ASTStatus SkipUntil(tok T1, tok T2 = tok::MAX);
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
  ASTStatus SkipSingle();
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
  static Scope *CreateScope(ScopeKind kind, ASTContext &sc,
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
