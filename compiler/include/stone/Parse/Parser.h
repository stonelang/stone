#ifndef STONE_PARSE_PARSER_H
#define STONE_PARSE_PARSER_H

#include "stone/Basic/StableHasher.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/Parsing.h"
#include "stone/Support/StatisticEngine.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTNode.h"
#include "stone/Syntax/ASTOptions.h"
#include "stone/Syntax/Attribute.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxResult.h"

#include "llvm/Support/Timer.h"

#include <memory>

namespace stone {

class BraceStmt;
class Parser;
class Scope;
class PairDelimiterBalancer;
class ParsingDecl;
class CodeCompletionCallbacks;

class Parser final {

  // friend PairDelimiterBalancer;

  std::unique_ptr<Lexer> lexer;

  ASTContext &astContext;
  SourceFile &sf;
  DeclContext *curDC;

  /// This is the current curTok being considered by the parser.
  Token curTok;

  /// Leading trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef leadingTrivia;

  /// Trailing trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef trailingTrivia;

  /// The location of the previous tok.
  SrcLoc prevTokLoc;

  // PairDelimiterCount pairDelimiterCount;

  ScopeCache scopeCache;

  // UInt16 ParenCount = 0;
  // UInt16 BraceCount = 0;
  // UInt16 BrackeCount = 0;

  CodeCompletionCallbacks *codeCompletionCallbacks = nullptr;

private:
  // Identifiers
  // mutable Identifier *importIdentifier;
  // mutable Identifier *moduleIdentifier;

  Parser(SourceFile &sourceFile, ASTContext &astContext,
         std::unique_ptr<Lexer> lexer);

public:
  Parser(SourceFile &sourceFile, ASTContext &astContext);
  ~Parser();

  // private:
  //   ImportSpecifier importSpecifier;
  //   StructSpecifier structSpecifier;
  //   PublicSpecifer  publicSpecifier;

public:
  bool HasCodeCompletionCallbacks() {
    return codeCompletionCallbacks != nullptr;
  }
  void SetCodeCompletionCallbacks(CodeCompletionCallbacks *callbacks) {
    codeCompletionCallbacks = callbacks;
  }
  CodeCompletionCallbacks *GetCodeCompletionCallbacks() {
    return codeCompletionCallbacks;
  }

public:
  Lexer &GetLexer() { return *lexer; }
  const Token &GetTok() const { return curTok; }
  ASTContext &GetASTContext() { return astContext; }

  /// The SourceFile is the current decl-context
  DeclContext *GetCurDeclContext() { return curDC; }

  /// The current curTok hash, or \c None if the parser isn't computing a hash
  /// for the curTok stream.
  std::optional<StableHasher> currentTokenHash;

public:
  void RecordTokenHash(const Token Tok) {
    if (!Tok.GetText().empty())
      RecordTokenHash(Tok.GetText());
  }
  void RecordTokenHash(StringRef curTok);

public:
  bool IsTopLevelDeclSpecifier();
  void ParseTopLevelDecls(llvm::SmallVector<SyntaxResult<Decl>> &results);
  SyntaxResult<Decl> ParseTopLevelDecl(ParsingDecl &collector);
  SyntaxResult<Decl> ParseDecl(ParsingDecl &parsingDecl);

public:
  void ParseDeclName();
  // TODO: Param should be constant
  SyntaxResult<Decl> ParseVarDecl(ParsingDecl &parsingDecl);
  SyntaxResult<Decl> ParseAutoDecl(ParsingDecl &parsingDecl);

public:
  // === Collectors === ///

  /// Decl
  SyntaxStatus CollectDeclSpecifier(ParsingDecl &parsingDecl);

  /// using
  SyntaxStatus CollectImportSpecifier(ParsingDecl &parsingDecl);

  /// private, internal, public
  SyntaxStatus CollectAccessSpecifier(ParsingDecl &parsingDecl);

  bool IsTypeChunk(const Token &tk);

  /// T*, T&
  SyntaxStatus CollectTypeChunk(ParsingDecl &parsingDecl);
  SyntaxStatus CollectTypeChunks(ParsingDecl &parsingDecl);

  /// Basic types: int, uint, etc.
  SyntaxStatus CollectBasicTypeSpecifier(ParsingDecl &parsingDecl);

  /// struct, class, and enum
  SyntaxStatus CollectNominalTypeSpecifier(ParsingDecl &parsingDecl);

  /// const, mutable, immutable, and pure.
  SyntaxStatus CollectTypeQualifiers(ParsingDecl &parsingDecl);
  SyntaxStatus CollectTypeQualifier(ParsingDecl &parsingDecl);

  /// new, and delete
  SyntaxStatus CollectTypeOperator(ParsingDecl &parsingDecl);

  /// extern, static
  SyntaxStatus CollectStorageSpecifier(ParsingDecl &parsingDecl);

  /// fun
  SyntaxStatus CollectFunctionSpecifier(ParsingDecl &parsingDecl);

public:
  // === Type Parsing ===//

  bool IsBasicType(tok kind) const;

  // TODO: Passing ParsingDecl -- may just want to pass the
  // Type parsingDecls in the furture. This is ok for now.
  Type ParseType(ParsingDecl &parsingDecl, Diag<> diagID);
  Type ParseFunctionType(ParsingDecl &parsingDecl, Diag<> diagID);
  Type ParsePointerType(ParsingDecl &parsingDecl, Diag<> diagID);
  Type ParseReferenceType(ParsingDecl &parsingDecl, Diag<> diagID);
  Type ParseDeclResultType(ParsingDecl &parsingDecl, Diag<> diagID);
  Type ParseBasicType(ParsingDecl &parsingDecl, Diag<> diagID);
  Type GetBasicType(TypeSpecifierKind kind);
  Type ParseIdentifierType(ParsingDecl &parsingDecl, Diag<> diagID);

public:
  //== fun ==//
  SyntaxResult<Decl> ParseFunDecl(ParsingDecl &parsingDecl);

private:
  SyntaxStatus ParseFunctionSignature(ParsingDecl &parsingDecl,
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

  SyntaxStatus ParseFunctionArguments(ParsingDecl &parsingDecl);
  SyntaxStatus ParseFunctionBody(ParsingDecl &parsingDecl,
                                 FunctionDecl &functionDecl);

  BraceStmt *ParseFunctionBodyImpl(ParsingDecl &parsingDecl,
                                   FunctionDecl &funDecl);

public:
  //== using ==//
  SyntaxResult<Decl> ParseImportDecl(ParsingDecl &parsingDecl);

public:
  //== struct ==//
  SyntaxResult<Decl> ParseStructDecl(ParsingDecl &parsingDecl);

public:
  //== enum== //
  SyntaxResult<Decl> ParseEnumDecl(ParsingDecl &parsingDecl);

public:
  //== interface ==//
  SyntaxResult<Decl> ParseInterfaceDecl(ParsingDecl &parsingDecl);

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
  bool HasError() { return astContext.GetDiags().HasError(); }
  DiagnosticEngine &GetDiags() { return astContext.GetDiags(); }

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

// class TopLevelSpecifier {

// };

// class ImportSpecifier : public TopLevelSpecifier {
// public:
//   void Collect();
// };
// class PublicSpecifier : public TopLevelSpecifier {
// public:
//   void Collect() {
//     auto specifier = GetTopLevelSecifier();
//     specifier.Collect();
//   }
// };

} // namespace stone
#endif
