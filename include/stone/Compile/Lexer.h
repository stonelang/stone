#ifndef STONE_COMPILE_LEXER_H
#define STONE_COMPILE_LEXER_H

#include "stone/Basic/Basic.h"
#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/SrcLoc.h"
#include "stone/Compile/Token.h"
#include "stone/Compile/Trivia.h"

namespace stone {

class SrcID;
class SrcMgr;
class Token;
class LangOptions;
class LexerPipeline;

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
  LexerStats(const Lexer &lexer, Basic &basic)
      : Stats("lexer statistics:", basic), lexer(lexer) {}
  void Print() override;
};

class Lexer final {
  friend LexerStats;
  const SrcID srcID;
  SrcMgr &sm;

  Basic &basic;
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

  LexerPipeline *pipeline = nullptr;

private:
  enum class NullCharType {
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
  Lexer(const SrcID srcID, SrcMgr &sm, Basic &basic,
        LexerPipeline *pipeline = nullptr);
  void Init(unsigned startOffset, unsigned endOffset);

public:
  void SetPipeline(LexerPipeline *p) { pipeline = p; }

  // TODO:
  bool ShouldKeepComments() const { return false; }

private:
  void Lex();
  void LexTrivia(Trivia trivia, bool isForTrailingTrivia);
  void LexIdentifier();
  void LexNumber();
  void LexHexNumber();
  void LexStrLiteral();
  unsigned LexChar(const char *&curPtr, char stopQuote, bool emitDiagnostics,
                   bool isMultilineString, unsigned customDelimiterLen);
  unsigned LexUnicodeEscape(const char *&curPtr, Basic *basic);

  void Diagnose();
  void MakeTok(tk::Type ty, const char *tokenStart);

  tk::Type GetIdentifierType(llvm::StringRef tokStr);

  void GoBack() { --curPtr; }
  void GoForward() { ++curPtr; }

public:
  void Lex(Token &result);
  void Lex(Token &result, Trivia &leading, Trivia &trailing);
  /// Returns it should be tokenize.
  bool LexAlien(bool emitDiagnosticsIfToken);

  Token &Peek() { return nextToken; }
  SrcID GetSrcID() { return srcID; }
  NullCharType GetNullCharType(const char *data) const;

public:
  template <typename... DiagArgTypes, typename... ArgTypes>
  InFlightDiagnostic Diagnose(const char *locPtr, Diag<DiagArgTypes...> DiagID,
                              ArgTypes &&...Args) {

    basic.GetDiagEngine().Diagnose(SrcLoc::GetFromPtr(locPtr), DiagID,
                                   std::forward<ArgTypes>(Args)...);
  }

private:
  void SkipToEndOfLine(bool eatNewline);
  void SkipSlashSlashComment(bool eatNewline);
};
} // namespace syn
} // namespace stone
#endif
