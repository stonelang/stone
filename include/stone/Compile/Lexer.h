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

/// Given a pointer to the starting byte of a UTF8 character, validate it and
/// advance the lexer past it.  This returns the encoded character or ~0U if
/// the encoding is invalid.
uint32_t ValidateUTF8CharAndAdvance(const char *&startOfByte, const char *end);

class Lexer;
class LexerStats final : public Stats {
  const Lexer &lexer;

public:
  LexerStats(const Lexer &lexer, Context &ctx)
      : Stats("lexer statistics:", ctx), lexer(lexer) {}

  const Lexer &GetLexer() { return lexer; }
  void Print() override;
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

  void MakeTok(tk::Kind ty, const char *tokenStart);

  tk::Kind GetIdentifierType(llvm::StringRef tokStr);

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
