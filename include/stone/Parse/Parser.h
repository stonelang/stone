#ifndef STONE_PARSE_PARSER_H
#define STONE_PARSE_PARSER_H

#include "stone/AST/ASTContext.h"
#include "stone/AST/ASTNode.h"
#include "stone/AST/ASTScope.h"
#include "stone/AST/Attribute.h"
#include "stone/AST/DiagnosticsParse.h"
#include "stone/AST/Expr.h"
#include "stone/AST/Identifier.h"
#include "stone/AST/Module.h"
#include "stone/AST/Stmt.h"
#include "stone/Basic/StableHasher.h"
#include "stone/Diag/DiagnosticParseKind.h"

#include "stone/Parse/CodeCompletionCallbacks.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/ParserResult.h"
#include "stone/Support/Statistics.h"

#include "llvm/Support/Timer.h"

#include <memory>

namespace stone {

class BraceStmt;
class Parser;
class ParsingDeclOptions;
class ParsingDeclSpec;
class ParsingTypeSpec;
class ParsingBuiltinTypeSpec;
class ParsingFunTypeSpec;
class ParsingStructTypeSpec;
class ParsingIdentifierTypeSpec;
class CodeCompletionCallbacks;

using ParsingScopeCache = llvm::SmallVector<ASTScope *, 16>;

class ParsingScope final {

  Parser &self;
  llvm::StringRef description;

public:
  ParsingScope(const ParsingScope &) = delete;
  void operator=(const ParsingScope &) = delete;

public:
  // ParseScope - Construct a new object to manage a scope in the
  // parser Self where the new Scope is created with the flags
  // ScopeFlags, but only when we aren't about to enter a compound statement --
  // may just pass Scope
  ParsingScope(Parser &self, ASTScopeKind kind,
               llvm::StringRef description = llvm::StringRef());
  ~ParsingScope();

private:
  /// EnterScope - start a new scope.
  void EnterScope(ASTScopeKind kind);

  /// ExitScope - pop a scope off the scope stack.
  void ExitScope();

  llvm::StringRef GetDescription() { return description; }
};

enum class ParsingNotification : uint8_t {
  None,
  DeclCreated,
  StmtCreated,
  ExprCreated,
  TokenConsumed,
};

class ParsingPosition final {
  friend class Parser;
  LexerState lexingState;
  /// TODO: prevTokLoc
  SrcLoc prevLoc;

  ParsingPosition(LexerState lexingState, SrcLoc prevLoc)
      : lexingState(lexingState), prevLoc(prevLoc) {}

public:
  ParsingPosition() = default;
  ParsingPosition &operator=(const ParsingPosition &) = default;

  bool isValid() const { return lexingState.IsValid(); }
};

class ParsingPunctuatorPair {
  uint8_t count;

public:
  enum class Kind : uint8_t {
    None = 0,
    Bracket,
    Paren,
    Brace,
  };
  ParsingPunctuatorPair::Kind kind;

public:
  ParsingPunctuatorPair(ParsingPunctuatorPair::Kind kind, Parser &parser)
      : kind(kind), count(0) {}

  ~ParsingPunctuatorPair() {}

public:
  ParsingPunctuatorPair::Kind GetKind() { return kind; }
  bool IsEven() { return ((count % 2) == 0); }
  void Increment() { ++count; }
};

/// Describes a parsed declaration name.
struct ParsedDeclName final {
  /// The name of the context of which the corresponding entity should
  /// become a member.
  StringRef ContextName;

  /// The base name of the declaration.
  StringRef BaseName;

  /// The argument labels for a function declaration.
  SmallVector<StringRef, 4> ArgumentLabels;

  /// Whether this is a function name (vs. a value name).
  bool IsFunctionName = false;

  bool IsSubscript = false;

  /// For a declaration name that makes the declaration into an
  /// instance member, the index of the "Self" parameter.
  std::optional<unsigned> ThisIndex;

  /// Determine whether this is a valid name.
  explicit operator bool() const { return !BaseName.empty(); }

  /// Whether this declaration name turns the declaration into a
  /// member of some named context.
  bool isMember() const { return !ContextName.empty(); }

  /// Whether the result is translated into an instance member.
  bool isInstanceMember() const {
    return isMember() && static_cast<bool>(ThisIndex);
  }

  /// Whether the result is translated into a static/class member.
  bool isClassMember() const {
    return isMember() && !static_cast<bool>(ThisIndex);
  }

  /// Whether this is an operator.
  bool isOperator() const { return Lexer::isOperator(BaseName); }

  /// Form a declaration name from this parsed declaration name.
  DeclName FormDeclName(ASTContext &ctx, bool isSubscript = false,
                        bool isCxxClassTemplateSpec = false) const;
};

class Parser final {

  ASTContext &astContext;

  SourceFile &sourceFile;

  /// The declaration context which is the source file
  DeclContext *curDC;

  std::unique_ptr<Lexer> lexer;

  // This is the previous token pasrsed by the parser.
  Token prevTok;

  /// This is the current token being considered by the parser.
  Token curTok;

  /// The location of the previous tok.
  SrcLoc prevTokLoc;

  /// Leading trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef leadingTrivia;

  /// Trailing trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef trailingTrivia;

  /// The current curTok hash, or \c None if the parser isn't computing a hash
  /// for the curTok stream.
  std::optional<StableHasher> currentTokenHash;

  /// The code completion call back
  CodeCompletionCallbacks *codeCompletionCallbacks = nullptr;

  mutable llvm::BumpPtrAllocator allocator;

  ParsingScopeCache parsingScopeCache;

private:
  Parser(SourceFile &sourceFile, ASTContext &astContext,
         std::unique_ptr<Lexer> lexer);

public:
  Parser(SourceFile &sourceFile, ASTContext &astContext);
  ~Parser();

public:
  /// Return the lexer
  Lexer &GetLexer() { return *lexer; }

  /// Return the ASTContext
  ASTContext &GetASTContext() { return astContext; }

  /// Return the current DeclContext which is the SourceFile
  DeclContext *GetCurDeclContext() { return curDC; }

  /// Return the current source file that is being paresed and populated.
  SourceFile &GetSourceFile() { return sourceFile; }

  /// Return the diagnostics
  DiagnosticEngine &GetDiags() { return astContext.GetDiags(); }

  ///\return the current token
  const Token &GetCurTok() const { return curTok; }

  /// save the previous token
  void SetPrevTok(Token tok) { prevTok = tok; }

  /// get the previous token
  const Token &GetPrevTok() const { return prevTok; }

  ///\return true if there exist a callback
  bool HasCodeCompletionCallbacks() {
    return codeCompletionCallbacks != nullptr;
  }
  /// set the callback
  void SetCodeCompletionCallbacks(CodeCompletionCallbacks *callbacks) {
    codeCompletionCallbacks = callbacks;
  }
  /// get the callback
  CodeCompletionCallbacks *GetCodeCompletionCallbacks() {
    return codeCompletionCallbacks;
  }

public:
  /// Allocate memory from the ASTContext bump pointer.
  void *AllocateMemory(size_t bytes, unsigned alignment = 8) const {
    if (bytes == 0) {
      return nullptr;
    }
    return GetAllocator().Allocate(bytes, alignment);
  }
  template <typename T> T *AllocateMemory(size_t num = 1) const {
    return static_cast<T *>(AllocateMemory(num * sizeof(T), alignof(T)));
  }
  void Deallocate(void *Ptr) const {}
  llvm::BumpPtrAllocator &GetAllocator() const { return allocator; }

  size_t GetTotalMemoryAllocated() const {
    return GetAllocator().getTotalMemory();
  }

private:
  void AddTopLevelDecl(ParserResult<Decl> result);
  void Lex(Token &result) { lexer->Lex(result); }
  void Lex(Token &result, llvm::StringRef &leading, llvm::StringRef &trailing) {
    lexer->Lex(result, leading, trailing);
  }

public:
  bool IsStartOfDecl();
  bool IsTopLevelDeclParsing();
  bool ParseTopLevelDecls();
  ParserResult<Decl> ParseTopLevelDecl(ParsingDeclSpec &spec);

  // ParserStatus ParseDeclSpec(ParsingDeclSpec &spec);

  // ParserStatus ParseQualSpec(DeclSpec &spec);
  // ParserStatus ParseTypeSpec(DeclSpec &spec);
  // ParserStatus ParseBuiltinTypeSpec(DeclSpec &spec);

  ParserResult<Decl> ParseDecl(ParsingDeclSpec &spec);
  ParserResult<ImportDecl> ParseImportDecl(ParsingDeclSpec &spec);

public:
  ParserResult<FunDecl> ParseFunDecl(ParsingDeclSpec &spec);
  ParserResult<StructDecl> ParseStructDecl(ParsingDeclSpec &spec);

private:
  ParserStatus ParseFunctionSignature(ParsingDeclSpec &spec,
                                      Identifier identifierName);

  ParserStatus ParseFunctionArguments(ParsingDeclSpec &spec);

public:
  // ParserStatus ParseStorageSpec(ParsingDeclSpec &spec);
  // ParserStatus ParseVisibilitySpec(ParsingDeclSpec &spec);
  // ParserStatus ParseBuiltinTypeSpec(ParsingDeclSpec &spec);
  // ParserStatus ParseNominalTypeSpec(ParsingDeclSpec &spec);
  // ParserStatus ParseFunctionTypeSpec(ParsingDeclSpec &spec);

  ParserResult<VarDecl> ParseVarDecl(ParsingDeclSpec &spec);

public:
  ParserResult<ParsingTypeSpec> ParseType();
  ParserResult<ParsingTypeSpec> ParseType(Diag<> diagID);
  ParserResult<ParsingTypeSpec> ParseBuiltinType(Diag<> diagID);

private:
  TypeKind ResolveBuiltinTypeKind(tok kind);
  QualType ResolveBuiltinType(TypeKind typeKind);

public:
  ParserResult<ParsingTypeSpec> ParseFunctionType(Diag<> diagID);

  // ParserResult<ParsingType> ParseTypeIdentifier(ParsingType *Base);

  // Type ParseType(ParsingDeclSpec &spec, Diag<> diagID);

  // Type ParsePointerType(ParsingDeclSpec &spec, Diag<> diagID);
  // Type ParseReferenceType(ParsingDeclSpec &spec, Diag<> diagID);
  // Type ParseDeclResultType(ParsingDeclSpec &spec, Diag<> diagID);
  // Type ParseIdentifierType(ParsingDeclSpec &spec, Diag<> diagID);

  // Type ParseBuiltinType(ParsingDeclSpec &spec, Diag<> diagID);
  // Type GetCachedBuiltinType(BuiltinTypeRep::Kind kind);

public:
  ParsingBuiltinTypeSpec *CreateParsingBuiltinTypeSpec(TypeKind kind,
                                                       SrcLoc loc);
  ParsingFunTypeSpec *CreateParsingFunTypeSpec(SrcLoc loc);
  ParsingStructTypeSpec *CreateParsingStructTypeSpec(SrcLoc loc);
  ParsingIdentifierTypeSpec *CreateParsingIdentifierTypeSpec(SrcLoc loc);

public:
  /// Return a parsed decl name
  ParsedDeclName ParseDeclName(llvm::StringRef name) const;

  /// Form a Swift declaration name from its constituent parts.
  DeclName FormDeclName(ASTContext &ctx, llvm::StringRef baseName,
                        llvm::ArrayRef<llvm::StringRef> argumentLabels,
                        bool isFunctionName, bool isInitializer,
                        bool isSubscript = false);

public:
  /// Stop parsing now.
  void EndParsing() { curTok.SetKind(tok::eof); }
  /// Is at end of file.
  bool IsEOF() { return curTok.GetKind() == tok::eof; }
  bool IsParsing() { return (!IsEOF() && !HasError()); }
  bool HasError() { return astContext.GetDiags().hadAnyError(); }

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

  // Helpers
  const Token &PeekNextToken() const { return lexer->Peek(); }
  SrcLoc GetCurLoc() { return curTok.GetLoc(); }

  void RecordTokenHash(const Token Tok) {
    if (!Tok.GetText().empty())
      RecordTokenHash(Tok.GetText());
  }
  void RecordTokenHash(StringRef curTok);

public:
  // == Skipping ==/

  ParserStatus SkipUntil(tok T1, tok T2 = tok::LAST);
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
  ParserStatus SkipSingle();
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
  bool SkipUntilTokenOrEndOfLine(tok T1, tok T2 = tok::LAST);

  /// Skip a braced block (e.g. function body). The current curTok must be '{'.
  /// Returns \c true if the parser hit the eof before finding matched '}'.
  ///
  /// Set \c HasNestedTypeDeclarations to true if a curTok for a type
  /// declaration is detected in the skipped block.
  bool SkipBracedBlock(bool &hasNestedTypeDeclarations);

  /// Skip over any attribute.
  void SkipAnyAttribute();

public:
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
  ParserStatus ParseIdentifier(Identifier &result, SrcLoc &resultLoc);
  ParserStatus ParseIdentifierDeclName(Identifier &result, SrcLoc &loc);
  SrcLoc ConsumeIdentifier(Identifier &result);

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
  // /// EnterScope - start a new scope.
  // void EnterScope(ASTScopeKind scopeKind);

  // /// ExitScope - pop a scope off the scope stack.
  // void ExitScope();

  // ASTScope *CreateScope(ASTScopeKind kind, ASTScope *parentScope);

  // ASTScope *GetCurScope() const;
  // bool HasCurScope() const { return (parsingScopeCache.size() > 0); }
  // void PopCurScope() { parsingScopeCache.pop_back(); }
  // void PushCurScope(ASTScope *scope) { parsingScopeCache.push_back(scope); }

public:
  InFlightDiagnostic Diagnose(SrcLoc loc, Diag<> diagID);
  InFlightDiagnostic Diagnose(Token &curTok, Diag<> diagID);
};

} // namespace stone

#endif