#include "clang/Basic/CharInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ConvertUTF.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Unicode.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <queue>
#include <string>
#include <utility>
#include <vector>

enum class TextKind : uint8_t {
#define TOKEN(X) X,
#include "stone/Basic/TokenKind.def"
  TotalTextKinds
};
class TextToken {};
struct TextSlice {};

struct TextLexer {

  /// Pointer to the first character of the buffer, even in a lexer that
  /// scans a subrange of the buffer.
  const char *StartBuff;

  /// Pointer to one past the end character of the buffer, even in a lexer
  /// that scans a subrange of the buffer.  Because the buffer is always
  /// NUL-terminated, this points to the NUL terminator.
  const char *TailBuff;

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

  /// True if we should skip past a `#!` line at the start of the file.
  const bool IsHashbangAllowed;

  const CommentRetentionMode RetainComments;

  const TriviaRetentionMode TriviaRetention;

  /// The current leading trivia for the next token.
  ///
  /// The StringRef points into the source buffer that is currently being lexed.
  llvm::StringRef LeadingTrivia;

  /// The current trailing trivia for the next token.
  /// The StringRef points into the source buffer that is currently being lexed.
  llvm::StringRef TrailingTrivia;

  /// If this is not \c nullptr, all tokens after this point are treated as eof.
  /// Used to cut off lexing early when we detect that the nesting level is too
  /// deep.
  const char *LexerCutOffPoint = nullptr;


  TextLexer(
      unsigned BufferID, const SrcMgr &sm, llvm::raw_ostream &Diag,
      HashbangMode HashbangAllowed = HashbangMode::Disallowed,
      CommentRetentionMode RetainComments = CommentRetentionMode::None,
      TriviaRetentionMode TriviaRetention = TriviaRetentionMode::WithoutTrivia);

  TextLexer(unsigned BufferID, const stone::SrcMgr &SM,
            llvm::raw_ostream &Diag);

  /// Create a TextLexer that scans a subrange of the source buffer.
  TextLexer(unsigned BufferID, const stone::SrcMgr &sm, llvm::raw_ostream &Diag,
            HashbangMode HashbangAllowed, CommentRetentionMode RetainComments,
            TriviaRetentionMode TriviaRetention, unsigned Offset,
            unsigned EndOffset);

  void FormToken(TokenKind Kind, const char *TokStart) {
    assert(CurPtr >= StartBuff && CurPtr <= TailBuff &&
           "Current pointer out of range!");
  }
};

class TextParser {
  llvm::SmallVector<TextSlice> slices;

public:
  void AddSlice(TextSlice slice) { slices.push_back(slice); }
  void Parse() {}

public:
  TextParser(llvm::StringRef IntextString);
};

class TextFormatter {
public:
  void Format(llvm::ArrayRef<TextSlice> slices) {
    for (auto slice : slices) {
    }
  }
};
