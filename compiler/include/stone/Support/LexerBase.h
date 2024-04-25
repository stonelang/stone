#ifndef STONE_SUPPORT_LEXERBASE_H
#define STONE_SUPPORT_LEXERBASE_H

namespace stone {

class SrcLoc;
class SrcMgr;
class Token;

class LexerBase {
public:
  virtual ~LexerBase() {}

public:
  virtual Token GetTokenAtLoc(const SrcMgr &sm, SrcLoc loc) = 0;
  virtual SrcLoc GetLocForEndOfToken(const SrcMgr &sm, SrcLoc loc) = 0;

public:
  /// Implementation of getEncodedStringSegment. Note that \p Str must support
  /// reading one byte past the end.
  // static StringRef getEncodedStringSegmentImpl(
  //     StringRef Str, SmallVectorImpl<char> &Buffer, bool IsFirstSegment,
  //     bool IsLastSegment, unsigned IndentToStrip, unsigned CustomDelimiterLen);

  // /// Implementation of getEncodedStringSegment. Note that \p Str must support
  // /// reading one byte past the end.
  // static StringRef getEncodedStringSegmentImpl(
  //     StringRef Str, SmallVectorImpl<char> &Buffer, bool IsFirstSegment,
  //     bool IsLastSegment, unsigned IndentToStrip, unsigned CustomDelimiterLen);
};
} // namespace stone
#endif
