#ifndef STONE_ANALYSIS_LEXING_H
#define STONE_ANALYSIS_LEXING_H

#include "stone/Basic/SrcLoc.h"

namespace stone {

class LexingState final {
public:
  LexingState() {}
  bool IsValid() const { return loc.isValid(); }

  LexingState Advance(unsigned offset) const {
    assert(IsValid());
    return LexingState(loc.getAdvancedLoc(offset));
  }

private:
  explicit LexingState(SrcLoc loc) : loc(loc) {}
  SrcLoc loc;
  llvm::StringRef leadingTrivia;
  friend class Lexer;
};

class LexingCache final {
public:
  LexingCache();
  ~LexingCache();
};

} // namespace stone
#endif
