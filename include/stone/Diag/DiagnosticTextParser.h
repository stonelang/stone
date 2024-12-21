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

enum class TextTokenKind {
  Identifier,
  Select,
};

class TextToken {
public:
};
/// May want to think about creating a source buffer using SrcMgr
// and treating this like a source file to parse with tokens and identifiers
// TextParserToken TextParserTokenKind
class TextParser final {
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
  TextParser(unsigned BufferID, SrcMgr &SM);
  void ParseText();
  void ParseIdentifier();
  void ParseSelect();
};

} // namespace diags
} // namespace stone
#endif