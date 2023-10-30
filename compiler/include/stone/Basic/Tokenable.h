#ifndef STONE_BASIC_TOKENABLE_H
#define STONE_BASIC_TOKENABLE_H

namespace stone {

class SrcLoc;
class SrcMgr;

namespace syn {
class Token;
}

class Tokenable {
public:
  virtual ~Tokenable() {}

public:
  virtual syn::Token GetTokenAtLoc(const SrcMgr &sm, SrcLoc loc) = 0;
  virtual SrcLoc GetLocForEndOfToken(const SrcMgr &sm, SrcLoc loc) = 0;
};
} // namespace stone
#endif
