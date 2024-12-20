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
};
} // namespace stone
#endif
