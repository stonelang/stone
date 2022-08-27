#ifndef STONE_SYNTAX_SYNTAXOPTIONS_H
#define STONE_SYNTAX_SYNTAXOPTIONS_H

#include "stone/Basic/STDTypeAlias.h"

#include "llvm/ADT/PointerUnion.h"

namespace stone {

class SyntaxOptions final {
public:
  unsigned LexingLineIndentation = 4;

public:
  SyntaxOptions() {}
};
} // namespace stone
#endif
