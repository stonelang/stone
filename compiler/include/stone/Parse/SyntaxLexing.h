#ifndef STONE_PARSE_SYNTAXLEXING_H
#define STONE_PARSE_SYNTAXLEXING_H

#include "stone/Basic/SrcLoc.h"

namespace stone {
namespace syn {

class SyntaxLexingState final {
public:
  SyntaxLexingState() {}
  bool IsValid() const { return loc.isValid(); }

  SyntaxLexingState Advance(unsigned offset) const {
    assert(IsValid());
    return SyntaxLexingState(loc.getAdvancedLoc(offset));
  }

private:
  explicit SyntaxLexingState(SrcLoc loc) : loc(loc) {}
  SrcLoc loc;
  llvm::StringRef leadingTrivia;
  friend class Lexer;
};

class SyntaxLexingCache final {
public:
  SyntaxLexingCache();
  ~SyntaxLexingCache();
};

class SyntaxLexing final {
public:
  SyntaxLexingCache cache;
  SyntaxLexingState state;
};

} // namespace syn
} // namespace stone
#endif
