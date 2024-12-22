#ifndef STONE_DIAG_DIAGNOSTICTEXTPARSER_H
#define STONE_DIAG_DIAGNOSTICTEXTPARSER_H

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

enum class TextTokenKind {
  None = 0,
  Identifier,
  Select,
  S,
  Pipe,
  LBrace,
  RBrace,
  LParen,
  RParen,
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
  Const,
  Star,
  eof,
};

class TextToken {
  /// The token kind
  TextTokenKind kind;

  /// Whether this token is the first token on the line.
  unsigned atStartOfLine : 1;

  /// Whether this token is an escaped `identifier` token.
  unsigned escapedIdentifier : 1;

  /// Modifiers for string literals
  unsigned multilineString : 1;

  /// Length of custom delimiter of "raw" string literals
  unsigned customDelimiterLen : 8;

  // Padding bits == 32 - 11;
  /// text - The actual string covered by the token in the source buffer.
  llvm::StringRef text;

public:
  TextToken(TextTokenKind kind, StringRef text)
      : kind(kind), atStartOfLine(false), escapedIdentifier(false),
        multilineString(false), customDelimiterLen(0), text(text) {}

  TextToken() : TextToken(TextTokenKind::eof, StringRef()) {}

public:
  /// Determine whether this token occurred at the start of a line.
  bool IsAtStartOfLine() const { return atStartOfLine; }

  /// Set whether this token occurred at the start of a line.
  void SetAtStartOfLine(bool value) { atStartOfLine = value; }

public:
  bool IsSelect() const { return kind == TextTokenKind::Select; }
};
/// May want to think about creating a source buffer using SrcMgr
// and treating this like a source file to parse with tokens and identifiers
// DiagnosticTextParserToken DiagnosticTextParserTokenKind
class DiagnosticTextParser final {
  unsigned BufferID;
  SrcMgr &SM;

  // Pointer to the first character of the buffer, even in a lexer that
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

  TextToken NextToken;

private:
  void Initialize(unsigned Offset, unsigned EndOffset);
  void Lex();

  void Lex(TextToken &Result) {
    Result = NextToken;
    // if (Result.IsNot(tok::eof)){
    //   Lex();
    // }
  }
  /// Reset the lexer's buffer pointer to \p Offset bytes after the buffer
  /// start.
  void ResetToOffset(size_t Offset) {
    assert(BufferStart + Offset <= BufferEnd && "Offset after buffer end");
    CurPtr = BufferStart + Offset;
    Lex();
  }

public:
  DiagnosticTextParser(unsigned BufferID, SrcMgr &SM);
  void Parse();
  void ParseIdentifier();
  void ParseSelect();
};

} // namespace diags
} // namespace stone
#endif