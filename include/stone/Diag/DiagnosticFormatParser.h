#ifndef STONE_DIAG_DIAGNOSTICFORMATPARSER_H
#define STONE_DIAG_DIAGNOSTICFORMATPARSER_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/SrcLoc.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <cassert>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace stone {
class SrcMgr;
namespace diags {

struct DiagnosticFormatOptions {
  const std::string OpeningQuotationMark;
  const std::string ClosingQuotationMark;
  const std::string AKAFormatString;
  const std::string OpaqueResultFormatString;

  DiagnosticFormatOptions(std::string OpeningQuotationMark,
                          std::string ClosingQuotationMark,
                          std::string AKAFormatString,
                          std::string OpaqueResultFormatString)
      : OpeningQuotationMark(OpeningQuotationMark),
        ClosingQuotationMark(ClosingQuotationMark),
        AKAFormatString(AKAFormatString),
        OpaqueResultFormatString(OpaqueResultFormatString) {}

  DiagnosticFormatOptions()
      : OpeningQuotationMark("'"), ClosingQuotationMark("'"),
        AKAFormatString("'%s' (aka '%s')"),
        OpaqueResultFormatString("'%s' (%s of '%s')") {}

  /// When formatting fix-it arguments, don't include quotes or other
  /// additions which would result in invalid code.
  static DiagnosticFormatOptions formatForFixIts() {
    return DiagnosticFormatOptions("", "", "%s", "%s");
  }
};

enum class DiagnosticFormatTokenKind {
  None = 0,
  Identifier,
  Select,
  S,
  Pipe,
  LBrace,
  RBrace,
  LParen,
  RParen,
  LSquare,
  RSquare,
  Percent,
  IntegerLiteral,
  Char,
  MultiChar,
  StringLiteral,
  Tick,
  Comma,
  Bool,
  Unsigned,
  Decl,
  QualType,
  Const,
  Star,
  Semi,
  eof,
  Invalid,
};

struct DiagnosticFormatToken final {

  /// The token kind
  DiagnosticFormatTokenKind Kind = DiagnosticFormatTokenKind::Invalid;

  /// Whether this token is the first token on the line.
  unsigned AtStartOfLine : 1;

  /// text - The actual string covered by the token in the source buffer.
  llvm::StringRef InText;

  DiagnosticFormatToken(DiagnosticFormatTokenKind kind, StringRef InText)
      : Kind(Kind), AtStartOfLine(false), InText(InText) {}

  DiagnosticFormatToken()
      : DiagnosticFormatToken(DiagnosticFormatTokenKind::eof, StringRef()) {}

  bool IsSelect() const { return Kind == DiagnosticFormatTokenKind::Select; }
  bool IsS() const { return Kind == DiagnosticFormatTokenKind::S; }
  bool IsLBrace() const { return Kind == DiagnosticFormatTokenKind::LBrace; }
  bool IsInvalid() const { return Kind == DiagnosticFormatTokenKind::Invalid; }
};

class DiagnosticFormatLexer final {

  llvm::StringRef InText;

  SrcMgr &SM;

  /// The ID of the source in the SrcMgr
  unsigned BufferID;

  /// Pointer to the first character of the buffer, even in a lexer that
  /// scans a subrange of the buffer.
  const char *BufferStart;

  /// Pointer to one past the end character of the buffer, even in a lexer
  /// that scans a subrange of the buffer.  Because the buffer is always
  /// NUL-terminated, this points to the NUL terminator.
  const char *BufferEnd;

  /// Pointer to the artificial EOF that is located before BufferEnd.  Useful
  /// for lexing subranges of a buffer.
  const char *ArtificialEOF = nullptr;

  /// If non-null, points to the '\0' character in the buffer where we should
  /// produce a code completion token.
  const char *CodeCompletionPtr = nullptr;

  /// Points to BufferStart or past the end of UTF-8 BOM sequence if it exists.
  const char *ContentStart;

  /// Pointer to the next not consumed character.
  const char *CurPtr;

  DiagnosticFormatToken NextToken;

  /// The current leading trivia for the next token.
  ///
  /// The StringRef points into the source buffer that is currently being lexed.
  // llvm::StringRef LeadingTrivia;

  /// The current trailing trivia for the next token.
  /// The StringRef points into the source buffer that is currently being lexed.
  // llvm::StringRef TrailingTrivia;

  /// If this is not \c nullptr, all tokens after this point are treated as eof.
  /// Used to cut off lexing early when we detect that the nesting level is too
  /// deep.
  const char *LexerCutOffPoint = nullptr;

  DiagnosticFormatLexer(unsigned BufferID, llvm::StringRef InText, SrcMgr &SM);
  void Initialize(unsigned Offset, unsigned EndOffset);
  void Lex();

public:
  DiagnosticFormatLexer(llvm::StringRef InText, SrcMgr &SM);

public:
  DiagnosticFormatToken PrevToken;

public:
  //   void Lex(TextToken &Result) {
  //     Result = NextToken;
  //     // if (Result.IsNot(tok::eof)){
  //     //   Lex();
  //     // }
  //   }
  //   /// Reset the lexer's buffer pointer to \p Offset bytes after the buffer
  //   /// start.
  //   void ResetToOffset(size_t Offset) {
  //     assert(BufferStart + Offset <= BufferEnd && "Offset after buffer end");
  //     CurPtr = BufferStart + Offset;
  //     Lex();
  //   }

  void LexIdentifier();
  void ConstructToken(DiagnosticFormatTokenKind Kind, const char *TokStart);
};

/// May want to think about creating a source buffer using SrcMgr
// and treating this like a source file to parse with tokens and identifiers
// DiagnosticTextParserToken DiagnosticTextParserTokenKind
class DiagnosticFormatParser final {

  DiagnosticFormatToken CurTok;
  DiagnosticFormatLexer Lexer;

  void ParseIdentifier();
  void ParseSelect();
  void AddToken(DiagnosticFormatToken Token) { Tokens.push_back(Token); }

public:
  llvm::raw_ostream &Out;
  DiagnosticFormatOptions &FormatOpts;
  llvm::SmallVector<DiagnosticFormatToken> Tokens;

public:
  DiagnosticFormatParser(llvm::raw_ostream &Out, llvm::StringRef InText,
                         SrcMgr &SM, DiagnosticFormatOptions &FormatOpts);
  void Lex();
  void Parse();
};

} // namespace diags
} // namespace stone
#endif