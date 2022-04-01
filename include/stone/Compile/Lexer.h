#ifndef STONE_PARSE_LEXER_H
#define STONE_PARSE_LEXER_H

#include "stone/Compile/LexerCache.h"
#include "stone/Compile/Trivia.h"
#include "stone/Core/Context.h"
#include "stone/Core/DiagnosticEngine.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Core/Token.h"
#include "stone/Core/Tokenable.h"

namespace stone {

class SrcID;
class SrcMgr;
class SyntaxListener;

namespace syn {
class Token;


enum class TriviaRetentionMode {
  Without,
  With,
};

/// Kinds of conflict marker which the lexer might encounter.
enum class ConflictMarkerKind {
  /// A normal or diff3 conflict marker, initiated by at least 7 "<"s,
  /// separated by at least 7 "="s or "|"s, and terminated by at least 7 ">"s.
  Normal,
  /// A Perforce-style conflict marker, initiated by 4 ">"s,
  /// separated by 4 "="s, and terminated by 4 "<"s.
  Perforce
};

/// Given a pointer to the starting byte of a UTF8 character, validate it and
/// advance the lexer past it.  This returns the encoded character or ~0U if
/// the encoding is invalid.
uint32_t ValidateUTF8CharAndAdvance(const char *&startOfByte, const char *end);

class Lexer;
class LexerStats final : public Stats {
  const Lexer &lexer;

public:
  LexerStats(const Lexer &lexer)
      : Stats("lexer statistics:"), lexer(lexer) {}
public:
  void Print(ColorfulStream& stream) override;
};

struct PrincipalCtor {};

/// Lexer state can be saved/restored to/from objects of this class.
class LexerState {
public:
  LexerState() {}

  bool IsValid() const { return loc.isValid(); }

  LexerState Advance(unsigned offset) const {
    assert(IsValid());
    return LexerState(loc.getAdvancedLoc(offset));
  }

private:
  explicit LexerState(SrcLoc loc) : loc(loc) {}
  SrcLoc loc;
  llvm::Optional<Trivia> leadingTrivia;
  friend class Lexer;
};

class Lexer final : public Tokenable {
  friend LexerStats;

  const unsigned srcID;
  const SrcMgr &sm;
  Context &ctx;
  LexerCache cache;
  LexerState state;

  std::unique_ptr<LexerStats> stats;

  /// Pointer to the first character of the buffer, even in a lexer that
  /// scans a subrange of the buffer.
  const char *bufferStart;

  /// Pointer to one past the end character of the buffer, even in a lexer
  /// that scans a subrange of the buffer.  Because the buffer is always
  /// NUL-terminated, this points to the NUL terminator.
  const char *bufferEnd;

  /// Pointer to the artificial EOF that is located before BufferEnd.  Useful
  /// for lexing subranges of a buffer.
  const char *artificialEOF = nullptr;

  /// If non-null, points to the '\0' character in the buffer where we should
  /// produce a code completion token.
  const char *codeCompletionPtr = nullptr;

  /// Points to BufferStart or past the end of UTF-8 BOM sequence if it exists.
  const char *contentStart;

  /// Pointer to the next not consumed character.
  const char *curPtr;

  Token nextToken;

  /// The current leading trivia for the next token.
  ///
  /// This is only preserved if this Lexer was constructed with
  /// `TriviaRetentionMode::With`.
  Trivia leadingTrivia;

  /// The current trailing trivia for the next token.
  ///
  /// This is only preserved if this Lexer was constructed with
  /// `TriviaRetentionMode::With`.
  Trivia trailingTrivia;

  SyntaxListener *pipeline = nullptr;

  /// The location at which the comment of the next token starts. \c nullptr if
  /// the next token doesn't have a comment.
  const char *commentStart;

  /// If this is not \c nullptr, all tokens after this point are treated as eof.
  /// Used to cut off lexing early when we detect that the nesting level is too
  /// deep.
  const char *lexerCutOffPoint = nullptr;

private:
  enum class NullCharKind {
    /// String buffer terminator.
    BufferEnd,
    /// Embedded nul character.
    Embedded,
    /// Code completion marker.
    CodeCompletion
  };

public:
  // Making this public for now
  TriviaRetentionMode triviaRetention;

private:
  Lexer(const Lexer &) = delete;
  void operator=(const Lexer &) = delete;

public:
  Lexer(PrincipalCtor &, const unsigned srcID, const SrcMgr &sm, Context &ctx,
        SyntaxListener *pipeline = nullptr);

  Lexer(const unsigned srcID, const SrcMgr &sm, Context &ctx,
        SyntaxListener *pipeline = nullptr);

  void Initialize(unsigned startOffset, unsigned endOffset);

public:
  void SetPipeline(SyntaxListener *p) { pipeline = p; }

  // TODO:
  bool ShouldKeepComments() const { return false; }

  /// Reset the lexer's buffer pointer to \p Offset bytes after the buffer
  /// start.
  void ResetToOffset(size_t offset) {
    assert(bufferStart + offset <= bufferEnd && "Offset after buffer end");
    curPtr = bufferStart + offset;
    Lex();
  }

  /// Cut off lexing at the current position. The next token to be lexed will
  /// be an EOF token, even if there is still source code to be lexed.
  /// The current and next token (returned by \c peekNextToken ) are not
  /// modified. The token after \c NextToken will be the EOF token.
  void CutOff() {
    // If we already have a cut off point, don't push it further towards the
    // back.
    if (lexerCutOffPoint == nullptr || lexerCutOffPoint >= curPtr) {
      lexerCutOffPoint = curPtr;
    }
  }

  /// If a lexer cut off point has been set returns the offset in the buffer at
  /// which lexing is being cut off.
  llvm::Optional<size_t> LexingCutOffOffset() const {
    if (lexerCutOffPoint) {
      return lexerCutOffPoint - bufferStart;
    } else {
      return llvm::None;
    }
  }

 // /// Returns the lexer state for the beginning of the given token
 //  /// location. After restoring the state, lexer will return this token and
 //  /// continue from there.
 //  LexerState GetStateForBeginningOfTokenLoc(SrcLoc Loc) const;

 //  /// Returns the lexer state for the beginning of the given token.
 //  /// After restoring the state, lexer will return this token and continue from
 //  /// there.
 //  LexerState GetStateForBeginningOfToken(const Token &tk,
 //                                    const StringRef &LeadingTrivia = {}) const {

 //    // If the token has a comment attached to it, rewind to before the comment,
 //    // not just the start of the token.  This ensures that we will re-lex and
 //    // reattach the comment to the token if rewound to this state.
 //    SrcLoc tkStart = tk.getCommentStart();
 //    if (tkStart.isInvalid())
 //      tkStart = tk.getLoc();
 //    auto state = GetStateForBeginningOfTokenLoc(TokStart);
 //    if (TriviaRetention == TriviaRetentionMode::WithTrivia) {
 //      S.LeadingTrivia = LeadingTrivia;
 //    } else {
 //      S.LeadingTrivia = StringRef();
 //    }
 //    return S;
 //  }

 //  LexerState GetStateForEndOfTokenLoc(SourceLoc Loc) const {
 //    return LexerState(getLocForEndOfToken(SourceMgr, Loc));
 //  }


private:
  /// Main lexing loop
  void Lex();

  void LexTrivia(Trivia trivia, bool isForTrailingTrivia);

  void LexIdentifier();

  void LexNumber();

  void LexHexNumber();

  void LexStringLiteral(unsigned customDelimiterLen = 0);

  void LexEscapedIdentifier();

  unsigned LexChar(const char *&curPtr, char stopQuote, bool emitDiagnostics,
                   bool isMultilineString, unsigned customDelimiterLen);

  unsigned LexUnicodeEscape(const char *&curPtr, Context *ctx);

  void LexAlien();

  void PrintD();

  void MakeTok(tok ty, const char *tokenStart);

  tok GetIdentifierType(llvm::StringRef tokStr);

  void GoBack() { --curPtr; }
  void GoForward() { ++curPtr; }

public:
  void Lex(Token &result);
  void Lex(Token &result, Trivia &leading, Trivia &trailing);
  /// Returns it should be tokenize.
  bool LexAlien(bool emitDiagnosticsIfToken);

  Token &Peek() { return nextToken; }
  unsigned GetSrcID() { return srcID; }
  NullCharKind GetNullCharKind(const char *data) const;

public:
  template <typename... DiagArgTypes, typename... ArgTypes>
  InFlightDiagnostic PrintD(const char *locPtr, Diag<DiagArgTypes...> DiagID,
                            ArgTypes &&...Args) {
    // TODO:ctx.GetDiagEngine().PrintD(SrcLoc::GetFromPtr(locPtr), DiagID,
    //                               std::forward<ArgTypes>(Args)...);
  }

private:
  void SkipToEndOfLine(bool eatNewline);
  void SkipSlashSlashComment(bool eatNewline);

public:
  Token GetTokenAtLoc(const SrcMgr &sm, SrcLoc loc) override;
  SrcLoc GetLocForEndOfToken(const SrcMgr &sm, SrcLoc loc) override;
};
} // namespace syn
} // namespace stone
#endif
